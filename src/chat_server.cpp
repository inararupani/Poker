//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <algorithm>
#include <vector>
#include "asio.hpp"
#include "chat_message.hpp"
#include "game.hpp"
#include "json.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

using asio::ip::tcp;




//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

Deck* deck = NULL;


int gameStatus; // -1 means not started, 1 means round 1, 2 means round to swap, 3 means final round
int currentBet;
int totalPot;
int turn;
int swapCount;
std::vector <string> idlist;
std::map<std::string, std::string> playerInfo;
std::map<std::string, hand> handInfo;

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const chat_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {
  }

  void start()
  {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_header()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            // clear out the old buffer from the last read
            // a '\0' is a good value to make sure a string
            // is terminated
            for (unsigned int i=0;i<chat_message::max_body_length;i++)
            {
                read_msg_.body() [i] = '\0';
            }


            do_read_body();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
        
            nlohmann::json to_dealer = nlohmann::json::parse(std::string(read_msg_.body()));

            nlohmann::json to_player;  // represents the entire game state.  sent to all players
          
            to_player["chat"] = to_dealer["chat"];

            if(to_dealer["event"] != "chat"){
              to_player["current_bet"] = to_dealer["total_bet"];

              if(to_dealer["event"] == "start"){
                if(gameStatus >= 1){
                  to_player["chat"] = "Game is already started.";
                }
                else if(idlist.size() <= 1){
                  to_player["chat"] = "Not enough players to start the game.";
                }
                else{
                  gameStatus = 1;

				  to_player["turn"]["name"] = playerInfo.at(idlist.at(0));	
				  to_player["turn"]["uuid"] = idlist.at(0);	
				  
                  for(int i = 0; i < (int)idlist.size(); i++){
                    hand H;

                    for(int j = 0; j < 5; j++){
                      H.handOfCards.push_back(deck->get_card());
                    }
					
					
                    H.sequenceHand();
                    
                    handInfo.insert(pair<std::string, hand> (idlist.at(i), H));

                    to_player["hand"][idlist.at(i)] = {{"card1",H.handOfCards.at(0).generateCardName()},
                                                        {"card2",H.handOfCards.at(1).generateCardName()},
                                                        {"card3",H.handOfCards.at(2).generateCardName()},
                                                        {"card4",H.handOfCards.at(3).generateCardName()},
                                                        {"card5",H.handOfCards.at(4).generateCardName()}};

                  }
                }
              }
              else if(to_dealer["event"] == "ante"){
                totalPot++;
                std::cout << (to_dealer["from"]["uuid"]) << std::endl;
                idlist.push_back(to_dealer["from"]["uuid"]);
                
                playerInfo.insert(pair<std::string,std::string> (std::string(to_dealer["from"]["uuid"]), std::string(to_dealer["from"]["name"])));
              }
              else if(to_dealer["event"] == "call"){
                int tempPot = to_dealer["total_bet"];
                totalPot = totalPot + tempPot;
                
                if (turn >= (int)idlist.size() - 1){
                    gameStatus++;
                  	turn = 0;
                  	if(gameStatus >= 4){
						std::string best;
						
						std::map<std::string, hand>::iterator it = handInfo.begin();
						

						while (it != handInfo.end())
						{
							it->second.sequenceHand();
							it++;
						}
                  		
                  		best = idlist.at(0);
						for(int i = 1; i < (int)(idlist.size()); i++){
							if(compareHand(handInfo.at(best), handInfo.at(idlist.at(i))) < 0){
								best = idlist.at(i);
								std::cout << "best hand changed" << std::endl;
							}
						}
						
						to_player["winner"] = playerInfo.at(best);
						to_player["winner_hand"] = getRank(handInfo.at(best));
						to_player["prize"]["amount"] = std::to_string(totalPot);
						to_player["prize"]["uuid"] = best;
						
						totalPot = 0;
						currentBet = 0;
						gameStatus = -1;
						swapCount = 0;
						
						
						idlist.clear();
						playerInfo.clear();
						handInfo.clear();

						
						
						
						
                  	}
                }
                
                else{
                	turn++;
                }
                
				if(gameStatus != -1){
					to_player["turn"]["name"] = playerInfo.at(idlist.at(turn));
					to_player["turn"]["uuid"] = idlist.at(turn);
				}
              }
              else if(to_dealer["event"] == "bet"){
                int tempPot = to_dealer["total_bet"];
                totalPot = totalPot + tempPot;
                
	            turn++;               

				to_player["turn"]["name"] = playerInfo.at(idlist.at(turn));
				to_player["turn"]["uuid"] = idlist.at(turn);
              }
              else if(to_dealer["event"] == "raise"){
                int tempPot = to_dealer["total_bet"];
                totalPot = totalPot + tempPot;
                
                std::rotate(idlist.begin(), idlist.begin() + turn ,idlist.end());
                
    
	            turn = 1;
              }
              else if(to_dealer["event"] == "swap"){
              	swapCount++;
              	string cardSwaps = to_dealer["swap_cards"];
              	
              	if(atoi(cardSwaps.c_str()) == 0){
              		std::cerr << "non-swapped" << std::endl;
              	}
              	else{
              		string temp = to_dealer["from"]["uuid"];
              		for(int i = 0; i < (int)cardSwaps.length(); i++){
              			handInfo.at(temp).handOfCards.at(atoi(cardSwaps.substr(i,1).c_str())-1) = deck->get_card();
              			to_player["hand"][temp]["card" + cardSwaps.substr(i,1)] = handInfo.at(temp).handOfCards.at(atoi(cardSwaps.substr(i,1).c_str())-1).generateCardName();
              		}
              	}
              	
              	if(swapCount == (int)idlist.size()){
              		turn = 0;
              		gameStatus++;
              		to_player["current_bet"] = 0;
              	}
              
              	
              }
                if(gameStatus != -1){
                	to_player["turn"]["name"] = playerInfo.at(idlist.at(turn));
                	to_player["turn"]["uuid"] = idlist.at(turn);
              	}

            }
            
            to_player["total_pot"] = std::to_string(totalPot);
            
			to_player["game_round"] = std::to_string(gameStatus);

        
        	/*    
        	to_player["turn"] = "3f96b414-9ac9-40b5-8007-90d0e771f0d0";   // UUID of the current player. 
            to_player["chat"] = to_dealer["chat"];
            to_player["dealer_comment"] = "fred has raised and received 2 new cards";
            to_player["recommended_play"] = "you should fold";
            to_player["hand"] = { 
                  {{"bet",1},{"current_bet",10}, {"uuid","3f96b414-9ac9-40b5-8007-90d0e771f0d0"} , {"name","Bud"} ,{"cards",{"acespades","10hearts","9clubs","2diamonds","kinghearts"}}},
                  {{"bet",2},{"current_bet",1}, {"uuid","3f96b414-9ac9-40b5-8007-20d0e771f0d0"} , {"name","Donald"} ,{"cards",{"acehearts","10spades","9clubs","2clubs","jackhearts"}}},
                  {{"bet",5},{"current_bet",5}, {"uuid","3f96b414-9ac9-40b5-8007-30d0e771f0d0"} , {"name","Ann"} ,{"cards",{"aceclubs","10diamonds","9clubs","2hearts","queenhearts"}}},
                  {{"bet",10},{"current_bet",0}, {"uuid","3f96b414-9ac9-40b5-8007-40d0e771f0d0"} , {"name","Melania"} ,{"cards",{"acediamonds","10clubs","9clubs","2spades","kinghearts"}}}
                               };
 
            //std::cout << "to player:" << std::endl;
            //std::cout << to_player.dump(2) << std::endl;
            */
            
            std::string t = to_player.dump();
            chat_message sending;
            if (t.size() < chat_message::max_body_length)
            {
              std::cout << "the size string being sent is " << t.size() << std::endl;
              memcpy( sending.body(), t.c_str(), t.size() );
              sending.body_length(t.size());
              sending.encode_header();
              room_.deliver(sending);
            }
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(asio::io_context& io_context,
      const tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<chat_session>(std::move(socket), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  deck = new Deck();
  deck->shuffle_deck();
  deck->shuffle_deck();
  totalPot = 0;
  gameStatus = -1;
  currentBet = 0;
  turn = 0;

  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    asio::io_context io_context;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

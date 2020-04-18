//
// chat_client.cpp
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
#include <thread>
#include <cstring>
#include <assert.h>
#include <vector>
#include "asio.hpp"
#include "chat_message.hpp"
#include <gtkmm.h>
#include "game.hpp"
#include "json.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <chrono>
#include <thread>

using asio::ip::tcp;




typedef std::deque<chat_message> chat_message_queue;


nlohmann::json to_dealer;
int curr_bet; // the most recent bet amount to be 
int tot_pot;
Gtk::Label *fromView = NULL;
Gtk::Label *totPot = NULL;
Gtk::Label *currBet = NULL;
Gtk::Label *currTurn = NULL;
player *curr_player = NULL;
std::string chatBox[5];
std::vector <Gtk::Image*> cardsImage;
bool turn;


class chat_client
{
public:
  chat_client(asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(const chat_message& msg)
  {
    asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();

          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, tcp::endpoint)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](std::error_code ec, std::size_t /*length*/)
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
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
          	nlohmann::json to_player = nlohmann::json::parse(std::string(read_msg_.body()));
          	
			if(!to_player["game_round"].empty()){
				string gameStatus = to_player["game_round"];
				std::cout << gameStatus << std::endl;
			}
			
			
          	if(!to_player["turn"]["name"].empty() && !to_player["turn"]["uuid"].empty()){          		
          		currTurn->set_markup("Turn: " + std::string(to_player["turn"]["name"]));   
          		if(std::string(to_player["turn"]["uuid"]) == curr_player->id){
          			turn = true;
          		}
          		else{
          			turn = false;
          		}        		
          	}

            std::cerr << to_player["hand"][curr_player->id]["card1"] << std::endl;
            if(!to_player["total_pot"].empty()){
            	totPot->set_markup("<b>" + std::string(to_player["total_pot"]) + "</b>"); 
            }
            if(!to_player["current_bet"].empty()){
            	curr_bet = to_player["current_bet"];
            	currBet->set_markup("<b>" + to_string(curr_bet) + "</b>"); 
            }
            
            if(!to_player["hand"][curr_player->id]["card1"].empty()){
              std::cerr << "it is not null" << std::endl;
              std::string tempCard;
              tempCard = to_player["hand"][curr_player->id]["card1"];  
              cardsImage.at(0)->set("src/cards/" + tempCard + ".png");
			  //cardsImage.at(0)->set_markup(tempCard);	
			  
			  std::this_thread::sleep_for(std::chrono::milliseconds(500));
    		  //std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
					
              tempCard = to_player["hand"][curr_player->id]["card2"]; 
              cardsImage.at(1)->set("src/cards/" + tempCard + ".png");
              //cardsImage.at(1)->set_markup(tempCard);	
              
              std::this_thread::sleep_for(std::chrono::milliseconds(500));
    		  //std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));

              tempCard = to_player["hand"][curr_player->id]["card3"]; 
              cardsImage.at(2)->set("src/cards/" + tempCard + ".png");
              //cardsImage.at(2)->set_markup(tempCard);	
              
              std::this_thread::sleep_for(std::chrono::milliseconds(500));
    		  //std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));

              tempCard = to_player["hand"][curr_player->id]["card4"]; 
              cardsImage.at(3)->set("src/cards/" + tempCard + ".png");
              //cardsImage.at(3)->set_markup(tempCard);	
              
              std::this_thread::sleep_for(std::chrono::milliseconds(500));
    		  //std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));

              tempCard = to_player["hand"][curr_player->id]["card5"]; 
              cardsImage.at(4)->set("src/cards/" + tempCard + ".png");
              //cardsImage.at(4)->set_markup(tempCard);	
              
            }

            for(int i = 0; i < 4; i++){
            	chatBox[i] = chatBox[i+1];
            }
			
            chatBox[4] = to_player["chat"];
			
			      fromView->set_label(chatBox[0] + "\n" 
					         			+ chatBox[1] + "\n"
								        + chatBox[2] + "\n"
								        + chatBox[3] + "\n"
								        + chatBox[4]); 
							

            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](std::error_code ec, std::size_t /*length*/)
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
            socket_.close();
          }
        });
  }

private:
  asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

chat_client *c;

//gtk classes start
playerNameWindow::playerNameWindow() {
  set_title("Enter game");
  set_border_width(30);
  resize(400,200);
  //set_position(Gtk::WIN_POS_CENTER_ALWAYS);
  Box.set_spacing(10);

  nameLabel.set_text("Enter your name:");
  Box.pack_start(nameLabel);

  Name.set_placeholder_text("eg: John");
  Box.pack_start(Name);

  OK.set_label("Ok");
  OK.signal_clicked().connect(sigc::mem_fun(*this, &playerNameWindow::on_OK));
  Box.pack_start(OK);

  Box.show_all();
  add(Box);

}

playerNameWindow::~playerNameWindow() {}
void playerNameWindow::on_OK() {
  hide();

  string playerName = Name.get_text();

  player *p = new player(playerName);
  to_dealer["from"] = { {"uuid", p->id} , {"name",p->playerName} };
  curr_player = p;

  playerWindow w(p);
  Gtk::Main::run(w);
} // get player's name from entry, opens playerWindow


playerWindow::playerWindow(player *p):Player(p){
  temp1 = 0;
  temp5 = 0;
  temp25 = 0;
  set_title(Player->playerName + "'s Game Window");
  set_border_width(10);
  resize(600,400);
  Box.set_spacing(10);
  
  currTurnLabel = new Gtk::Label();
  currTurn = currTurnLabel;
  currTurnLabel->set_markup("Turn: ");
  Box.pack_start(*currTurnLabel);		
	
  cardsBox.set_spacing(10);
  Card1 = new Gtk::Image();
  cardsImage.push_back(Card1);
  //Card1->set_markup("blank");
  Card1->set("src/cards/blank_card.png");
  cardsBox.pack_start(*Card1);

  Card2 = new Gtk::Image();
  cardsImage.push_back(Card2);
  //Card2->set_markup("blank");
  Card2->set("src/cards/blank_card.png");
  cardsBox.pack_start(*Card2);

  Card3 = new Gtk::Image();
  cardsImage.push_back(Card3);
  Card3->set("src/cards/blank_card.png");
  //Card3->set_markup("blank");
  cardsBox.pack_start(*Card3);

  Card4 = new Gtk::Image();
  cardsImage.push_back(Card4);
  Card4->set("src/cards/blank_card.png");
  //Card4->set_markup("blank");
  cardsBox.pack_start(*Card4);

  Card5 = new Gtk::Image();
  cardsImage.push_back(Card5);
  Card5->set("src/cards/blank_card.png");
  //Card5->set_markup("blank");
  cardsBox.pack_start(*Card5);

  Box.pack_start(cardsBox);

  totalPot.set_markup("<b>TOTAL POT:</b>");
  Box.pack_start(totalPot);

  totPotLabel = new Gtk::Label();
  totPot = totPotLabel;
  totPotLabel->set_markup("<b>0</b>"); 
  Box.pack_start(*totPotLabel);

  currentBet.set_markup("<b>CURRENT BET:</b>");	
  Box.pack_start(currentBet);

  currBetLabel = new Gtk::Label();
  currBet = currBetLabel;
  currBetLabel->set_markup("<b>" + to_string(curr_bet) + "</b>"); 
  Box.pack_start(*currBetLabel);

  balanceLabel.set_markup("Balance: " + to_string(Player->balance) + 
                              "\t$1: " + to_string(Player->chip1) + 
                              "\t$5: " + to_string(Player->chip5) + 
                              "\t$25: " + to_string(Player->chip25));

  Box.pack_start(balanceLabel);

  yourBet.set_markup("<b>YOUR BET</b>");
  Box.pack_start(yourBet);

  Amount.set_markup("<b>0</b>");
  Box.pack_start(Amount);

  Start.set_label("Start");
  Start.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Start));
  startBox.pack_start(Start);

  Ante.set_label("Ante($1)");
  Ante.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Ante));
  startBox.pack_start(Ante);

  Box.pack_start(startBox);

  chip1.set_label("$1");
  chip1.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_chip_1));
  chipBox.pack_start(chip1);

  chip5.set_label("$5");
  chip5.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_chip_5));
  chipBox.pack_start(chip5);

  chip25.set_label("$25");
  chip25.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_chip_25));
  chipBox.pack_start(chip25);

  clear.set_label("Clear");
  clear.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Clear));
  chipBox.pack_start(clear);

  Help.set_label("Help");
  Help.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Help));
  chipBox.pack_start(Help);

  Box.pack_start(chipBox);

  Call.set_label("Call");
  Call.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Call));
  actionBox.pack_start(Call);
  
  Bet.set_label("Bet");
  Bet.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Bet));
  actionBox.pack_start(Bet);

  Raise.set_label("Raise");
  Raise.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Raise));
  actionBox.pack_start(Raise);

  Fold.set_label("Fold");
  Fold.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Fold));
  actionBox.pack_start(Fold);

  CardSwap.set_label("Swap");
  CardSwap.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_CardSwap));
  actionBox.pack_start(CardSwap);

  SitOut.set_label("Sit Out");
  SitOut.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_SitOut));
  actionBox.pack_start(SitOut);

  Box.pack_start(actionBox);
    
  Chat.set_placeholder_text("eg: Hi");
  Box.pack_start(Chat);

  Send.set_label("Send");
  Send.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Send));
  Box.pack_start(Send);

  chatHistory.set_markup("<b>CHAT HISTORY:</b>");
  Box.pack_start(chatHistory);

  chatLabel = new Gtk::Label();
  fromView = chatLabel;
  chatLabel->set_label(chatBox[0] + "\n" 
  						+ chatBox[1] + "\n"
  						+ chatBox[2] + "\n"
  						+ chatBox[3] + "\n"
  						+ chatBox[4]);
  						

  Box.pack_start(*chatLabel);

  Exit.set_label("Exit");
  Exit.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Exit));
  Box.pack_start(Exit);

  Box.show_all();
  add(Box);
}

playerWindow::~playerWindow() {}

void playerWindow::on_Start() {
  if(Player->status == false){
    Gtk::MessageDialog dialog(*this, "Can't start without ante.");
    dialog.run();
    dialog.hide();
  }
  else{
    to_dealer["event"] = "start";
    to_dealer["chat"] = Player->playerName + " started the game.";

    std::string t = to_dealer.dump();

    chat_message msg;

    msg.body_length(t.size());
    std::memcpy(msg.body(), t.c_str() , msg.body_length());
    msg.encode_header();
    c->write(msg);
  }
	
}

void playerWindow::on_Ante(){
	if(Player->chip1 <= 0 || Player->status == true){
		Gtk::MessageDialog dialog(*this, "You do not have enough $1 chips to ante or you have already anted.");
	  dialog.run();
	  dialog.hide();
	}
	else{
		Player->status = true; //player status is true when the player has already ante.
		to_dealer["event"] = "ante";
		to_dealer["chat"] = Player->playerName + " put ante $1.";

		std::string t = to_dealer.dump();

		chat_message msg;

		msg.body_length(t.size());
		std::memcpy(msg.body(), t.c_str() , msg.body_length());
		msg.encode_header();
		c->write(msg);

		Player->chip1--;
		Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;
		balanceLabel.set_markup("Balance: " + to_string(Player->balance) + 
		                    "\t$1: " + to_string(Player->chip1) + 
		                    "\t$5: " + to_string(Player->chip5) + 
		                    "\t$25: " + to_string(Player->chip25));

	}
}

void playerWindow::on_Menu() {}
void playerWindow::on_Help() {
	Gtk::MessageDialog dialog(*this, "Learn how to play 5 card draw:\nhttps://www.youtube.com/watch?v=UmtSUhSfyYE");
    dialog.run();
    dialog.hide();
}
void playerWindow::on_Call() {
	if(std::atoi(Amount.get_text().c_str()) == curr_bet && Player->status == true) {
		temp1 = 0;
		temp5 = 0;
		temp25 = 0;

		to_dealer["event"] = "call";
		to_dealer["total_bet"] = std::atoi(Amount.get_text().c_str());//bet done by user
		to_dealer["chat"] = Player->playerName + " called $" + Amount.get_text().c_str();

		Amount.set_markup("<b>0</b>");

		chat_message msg;
		std::string t = to_dealer.dump();

		msg.body_length(t.size());
		std::memcpy(msg.body(), t.c_str() , msg.body_length());
		msg.encode_header();
		c->write(msg);
	}
	else if(curr_bet && Player->status == false){
    Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
    dialog.run();
    dialog.hide();

  }
  else
  {
		Gtk::MessageDialog dialog(*this, "Your call doesn't match current bet.");
	    dialog.run();
	    dialog.hide();
	}


}
void playerWindow::on_Raise() {
	if(std::atoi(Amount.get_text().c_str()) > curr_bet && Player->status == true){
    temp1 = 0;
    temp5 = 0;
    temp25 = 0;

    to_dealer["event"] = "raise";
    to_dealer["total_bet"] = std::atoi(Amount.get_text().c_str());//bet done by user
    to_dealer["chat"] = Player->playerName + " raised to $" + Amount.get_text().c_str();

    Amount.set_markup("<b>0</b>");

    chat_message msg;
    std::string t = to_dealer.dump();

    msg.body_length(t.size());
    std::memcpy(msg.body(), t.c_str() , msg.body_length());
    msg.encode_header();
    c->write(msg);
  }
  else if(curr_bet && Player->status == false){
    Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
    dialog.run();
    dialog.hide();

  }
  else{
    Gtk::MessageDialog dialog(*this, "Raise should be greater than current bet.");
      dialog.run();
      dialog.hide();
  }

}
void playerWindow::on_Fold() {
	to_dealer["event"] = "fold";
}
void playerWindow::on_Bet() {
	 if(curr_bet == 0 && Player->status == true){
    temp1 = 0;
    temp5 = 0;
    temp25 = 0;

    to_dealer["event"] = "bet";
    to_dealer["total_bet"] = std::atoi(Amount.get_text().c_str());//bet done by user
    to_dealer["chat"] = Player->playerName + " bet $" + Amount.get_text().c_str();

    Amount.set_markup("<b>0</b>");

    chat_message msg;
    std::string t = to_dealer.dump();

    msg.body_length(t.size());
    std::memcpy(msg.body(), t.c_str() , msg.body_length());
    msg.encode_header();
    c->write(msg);
  }
  else if(curr_bet && Player->status == false){
    Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
    dialog.run();
    dialog.hide();

  }
  else{
    Gtk::MessageDialog dialog(*this, "Call current bet or raise.");
      dialog.run();
      dialog.hide();
  }
}
void playerWindow::on_CardSwap() {
	to_dealer["event"] = "request_cards";
}
void playerWindow::on_SitOut() {
	to_dealer["event"] = "sit_out";
	hide();
}
void playerWindow::on_Exit() {
  hide();
}


void playerWindow::on_Send(){

	to_dealer["event"] = "chat";
	to_dealer["chat"] =  Player->playerName + ": " + std::string((Chat.get_text()).c_str());

	Chat.set_text("");

	std::string t = to_dealer.dump();

	chat_message msg;

	msg.body_length(t.size());
	std::memcpy(msg.body(), t.c_str() , msg.body_length());
	msg.encode_header();
	c->write(msg);

}



void playerWindow::on_chip_1() {
  if(Player->chip1 <= 0){
      Gtk::MessageDialog dialog(*this, "Not enough $1 chips.");
      dialog.run();
      dialog.hide();
  }
  else{
    Player->chip1--;
    temp1++;
    Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

    balanceLabel.set_markup("Balance: " + to_string(Player->balance) + 
                                "\t$1: " + to_string(Player->chip1) + 
                                "\t$5: " + to_string(Player->chip5) + 
                                "\t$25: " + to_string(Player->chip25));
    
    std::string your_bet_s = Amount.get_text();
    int your_bet = std::atoi(your_bet_s.c_str());
    your_bet = your_bet + 1;
    Amount.set_markup("<b>" + to_string(your_bet) + "</b>");    
  }

  
}
void playerWindow::on_chip_5() {
  if(Player->chip5 <= 0){
    Gtk::MessageDialog dialog(*this, "Not enough $5 chips.");
    dialog.run();
    dialog.hide();
  }
  else{
    Player->chip5--;
    temp5++;
    Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

    balanceLabel.set_markup("Balance: " + to_string(Player->balance) + 
                                "\t$1: " + to_string(Player->chip1) + 
                                "\t$5: " + to_string(Player->chip5) + 
                                "\t$25: " + to_string(Player->chip25));
    
    std::string your_bet_s = Amount.get_text();
    int your_bet = std::atoi(your_bet_s.c_str());
    your_bet = your_bet + 5;
    Amount.set_markup("<b>" + to_string(your_bet) + "</b>");   
  }

}
void playerWindow::on_chip_25() {
  if(Player->chip25 <= 0){
    Gtk::MessageDialog dialog(*this, "Not enough $25 chips.");
    dialog.run();
    dialog.hide();
  }
  else{
    Player->chip25--;
    temp25++;
    Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

    balanceLabel.set_markup("Balance: " + to_string(Player->balance) + 
                                "\t$1: " + to_string(Player->chip1) + 
                                "\t$5: " + to_string(Player->chip5) + 
                                "\t$25: " + to_string(Player->chip25));
    
    std::string your_bet_s = Amount.get_text();
    int your_bet = std::atoi(your_bet_s.c_str());
    your_bet = your_bet + 25;
    Amount.set_markup("<b>" + to_string(your_bet) + "</b>");    
  }

}

void playerWindow::on_Clear(){
	Player->chip1 = Player->chip1 + temp1;
	Player->chip5 = Player->chip5 + temp5;
	Player->chip25 = Player->chip25 + temp25;

	temp1 = 0;
	temp5 = 0;
	temp25 = 0;

	Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

    balanceLabel.set_markup("Balance: " + to_string(Player->balance) + 
                            "\t$1: " + to_string(Player->chip1) + 
                            "\t$5: " + to_string(Player->chip5) + 
                            "\t$25: " + to_string(Player->chip25));



	Amount.set_markup("<b>0</b>");
}

//gtk classes end


int main(int argc, char* argv[])
{
	for(int i = 0; i < 5; i++){
		chatBox[i] = "";
	}


	curr_bet = 0;
  	to_dealer["total_bet"] = 0;
	turn = true;
	//gameStatus = -1;

	/*
    to_dealer["from"] = { {"uuid",} , {"name","Bud"} };
    to_dealer["event"] = "stand";        // "stand","hit","fold","raise","join","request_cards"
    to_dealer["cards_requested"] = 3;    // optional, number of cards requested, 1 to 5
    to_dealer["current_bet"] = 1.00;
    to_dealer["total_bet"] = 5.00;
    to_dealer["chat"] = std::string(line);
    */

	//clear temp1, temp5, temp25, in every button click
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    c = new chat_client(io_context, endpoints);
    assert(c);
    std::thread t([&io_context](){ io_context.run(); });

    Gtk::Main app(argc, argv);
    playerNameWindow w;
    Gtk::Main::run(w);

    //char line[chat_message::max_body_length + 1];
    
    /*
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      chat_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }
    */
    c->close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

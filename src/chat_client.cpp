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
#include "card.hpp"
#include "deck.hpp"
#include "hand.hpp"
#include "player.hpp"
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
Gtk::Label *balanceLbl = NULL;
player *curr_player = NULL;
std::string chatBox[5];
std::vector <Gtk::Image*> cardsImage;
int gameStatus;


bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](unsigned char c)
    {
        return !std::isdigit(c);
    }) == s.end();
}

bool valid_number(std::string numstr)
{
    if(numstr.length()>3)
    {
        return false;
    }
    else
    {
        if(numstr.length() == 1 && atoi(numstr.c_str()) == 0)
        {
            return true;
        }
        else
        {
            for(int i = 0; i < (int)numstr.length(); i++)
            {
                if(atoi(numstr.substr(i,1).c_str()) > 5 || atoi(numstr.substr(i,1).c_str())  < 1 )
                {
                    return false;
                }
            }
            return true;
        }
    }
}


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
        asio::post(io_context_, [this]()
        {
            socket_.close();
        });
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
                for (unsigned int i=0; i<chat_message::max_body_length; i++)
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



                if(!to_player["game_round"].empty())
                {
                    string tempStatus = to_player["game_round"];
                    gameStatus = atoi(tempStatus.c_str());
                }

                if(!to_player["turn"]["name"].empty() && !to_player["turn"]["uuid"].empty())
                {
                    currTurn->set_markup("Turn: " + std::string(to_player["turn"]["name"]));
                    if(std::string(to_player["turn"]["uuid"]) == curr_player->id)
                    {
                        curr_player->turn = true;
                    }
                    else
                    {
                        curr_player->turn = false;
                    }
                }

                if(!to_player["total_pot"].empty())
                {
                    totPot->set_markup("<b>" + std::string(to_player["total_pot"]) + "</b>");
                }
                if(!to_player["current_bet"].empty())
                {
                    curr_bet = to_player["current_bet"];
                    currBet->set_markup("<b>" + to_string(curr_bet) + "</b>");
                }

                if(!to_player["hand"][curr_player->id]["card1"].empty())
                {
                    std::string tempCard;
                    tempCard = to_player["hand"][curr_player->id]["card1"];
                    cardsImage.at(0)->set("src/cards/" + tempCard + ".png");
                    //cardsImage.at(0)->set_markup(tempCard);
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }

                if(!to_player["hand"][curr_player->id]["card2"].empty())
                {
                    std::string tempCard;
                    tempCard = to_player["hand"][curr_player->id]["card2"];
                    cardsImage.at(1)->set("src/cards/" + tempCard + ".png");
                    //cardsImage.at(1)->set_markup(tempCard);
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                if(!to_player["hand"][curr_player->id]["card3"].empty())
                {
                    std::string tempCard;
                    tempCard = to_player["hand"][curr_player->id]["card3"];
                    cardsImage.at(2)->set("src/cards/" + tempCard + ".png");
                    //cardsImage.at(2)->set_markup(tempCard);
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                if(!to_player["hand"][curr_player->id]["card4"].empty())
                {
                    std::string tempCard;
                    tempCard = to_player["hand"][curr_player->id]["card4"];
                    cardsImage.at(3)->set("src/cards/" + tempCard + ".png");
                    //cardsImage.at(3)->set_markup(tempCard);
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                if(!to_player["hand"][curr_player->id]["card5"].empty())
                {
                    std::string tempCard;
                    tempCard = to_player["hand"][curr_player->id]["card5"];
                    cardsImage.at(4)->set("src/cards/" + tempCard + ".png");
                }




                for(int i = 0; i < 4; i++)
                {
                    chatBox[i] = chatBox[i+1];
                }

                chatBox[4] = to_player["chat"];

                fromView->set_label(chatBox[0] + "\n"
                                    + chatBox[1] + "\n"
                                    + chatBox[2] + "\n"
                                    + chatBox[3] + "\n"
                                    + chatBox[4]);
                                    
                if(gameStatus == 2)
                {	
                	curr_player->checked = false;	
                }

                if(gameStatus == -1 && !to_player["winner"].empty())  //need to make 0 later
                {

                    if(to_player["prize"]["uuid"] == curr_player->id)
                    {
                        curr_player->balance = curr_player->balance + atoi(std::string(to_player["prize"]["amount"]).c_str());
                        curr_player->chip1 = curr_player->chip1 + atoi(std::string(to_player["prize"]["amount"]).c_str());
                        balanceLbl->set_markup("Balance: " + to_string(curr_player->balance) +
                                               "\t$1: " + to_string(curr_player->chip1) +
                                               "\t$5: " + to_string(curr_player->chip5) +
                                               "\t$25: " + to_string(curr_player->chip25));

                    }

                    for(int i = 0; i < 4; i++)
                    {
                        chatBox[i] = chatBox[i+1];
                    }
                    curr_player->swapped = false;
                    curr_player->checked = false;
                    curr_player->turn = false;
                    curr_player->status = false;

                    chatBox[4] = std::string(to_player["winner"]) + " is the winner with " + std::string(to_player["winner_hand"]);
                    currTurn->set_markup("Turn: None");

                    fromView->set_markup(chatBox[0] + "\n"
                                         + chatBox[1] + "\n"
                                         + chatBox[2] + "\n"
                                         + chatBox[3] + "\n"
                                         + "<b>" + chatBox[4] + "</b>");
                }


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
playerNameWindow::playerNameWindow()
{
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

void playerNameWindow::on_OK()
{
    hide();

    string playerName = Name.get_text();

    player *p = new player(playerName);
    to_dealer["from"] = { {"uuid", p->id}, {"name",p->playerName} };
    curr_player = p;

    playerWindow w(p);
    Gtk::Main::run(w);
} // get player's name from entry, opens playerWindow


playerWindow::playerWindow(player *p):Player(p)
{
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

    checkBox.pack_start(swap1);

    checkBox.pack_start(swap2);

    checkBox.pack_start(swap3);

    checkBox.pack_start(swap4);

    checkBox.pack_start(swap5);

    Box.pack_start(checkBox);

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

    balanceLabel  = new Gtk::Label();
    balanceLbl = balanceLabel;
    balanceLabel->set_markup("Balance: " + to_string(Player->balance) +
                             "\t$1: " + to_string(Player->chip1) +
                             "\t$5: " + to_string(Player->chip5) +
                             "\t$25: " + to_string(Player->chip25));

    Box.pack_start(*balanceLabel);

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

	Check.set_label("Check");
	Check.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Check));
	actionBox.pack_start(Check);

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

    Exit.set_label("Fold and Leave");
    Exit.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Exit));
    Box.pack_start(Exit);

    Box.show_all();
    add(Box);
}


void playerWindow::on_Start()
{
    if(Player->status == false)
    {
        Gtk::MessageDialog dialog(*this, "Can't start without ante.");
        dialog.run();
        dialog.hide();
    }
    else
    {
        to_dealer["event"] = "start";
        to_dealer["chat"] = Player->playerName + " started the game.";

        std::string t = to_dealer.dump();

        chat_message msg;

        msg.body_length(t.size());
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);
    }

}

void playerWindow::on_Ante()
{
    if(Player->chip1 <= 0 || Player->status == true)
    {
        Gtk::MessageDialog dialog(*this, "You do not have enough $1 chips to ante or you have already anted.");
        dialog.run();
        dialog.hide();
    }
    else
    {
        Player->status = true; //player status is true when the player has already ante.
        to_dealer["event"] = "ante";
        to_dealer["chat"] = Player->playerName + " put ante $1.";

        std::string t = to_dealer.dump();

        chat_message msg;

        msg.body_length(t.size());
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);

        Player->chip1--;
        Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;
        balanceLabel->set_markup("Balance: " + to_string(Player->balance) +
                                 "\t$1: " + to_string(Player->chip1) +
                                 "\t$5: " + to_string(Player->chip5) +
                                 "\t$25: " + to_string(Player->chip25));

    }
}

void playerWindow::on_Help()
{
    Gtk::MessageDialog dialog(*this, "Learn how to play 5 card draw:\nhttps://www.youtube.com/watch?v=UmtSUhSfyYE");
    dialog.run();
    dialog.hide();
}
void playerWindow::on_Call()
{
	
    if(curr_bet != 0 && std::atoi(Amount.get_text().c_str()) == curr_bet && Player->status == true && Player->turn == true && gameStatus != 2 && gameStatus !=-1) 
    {
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
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);
    }
    else if(Player->status == false)
    {
        Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
        dialog.run();
        dialog.hide();

    }
    else if(gameStatus == -1)
    {
		Gtk::MessageDialog dialog(*this, "Game hasn't started yet.");
        dialog.run();
        dialog.hide();    
    
    }

    else if(Player->turn == false)
    {
    	Gtk::MessageDialog dialog(*this, "It is not your turn.");
        dialog.run();
        dialog.hide();

    }
    else if(gameStatus == 2)
    {
    	Gtk::MessageDialog dialog(*this, "It is a swapping round.");
        dialog.run();
        dialog.hide();
    
    }
    else if(curr_bet == 0)
    {
        Gtk::MessageDialog dialog(*this, "Bet before you call.");
        dialog.run();
        dialog.hide();    
    }

    else if(std::atoi(Amount.get_text().c_str()) != curr_bet)
    {
        Gtk::MessageDialog dialog(*this, "Your call doesn't match current bet.");
        dialog.run();
        dialog.hide();
    }



}
void playerWindow::on_Raise()
{
    if(std::atoi(Amount.get_text().c_str()) > curr_bet && Player->status == true && Player->turn == true && gameStatus != 2 && gameStatus != -1)
    {
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
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);
    }
    else if(Player->status == false)
    {
        Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
        dialog.run();
        dialog.hide();

    }
    else if(gameStatus == -1)
    {
		Gtk::MessageDialog dialog(*this, "Game hasn't started yet.");
        dialog.run();
        dialog.hide();    
    
    }

    else if(Player->turn == false)
    {
    	Gtk::MessageDialog dialog(*this, "It is not your turn.");
        dialog.run();
        dialog.hide();
    }
    else if(gameStatus == 2)
    {
    	Gtk::MessageDialog dialog(*this, "It is a swapping round.");
        dialog.run();
        dialog.hide();
    
    }
    else if(std::atoi(Amount.get_text().c_str()) <= curr_bet)
    {
        Gtk::MessageDialog dialog(*this, "Raise should be greater than current bet.");
        dialog.run();
        dialog.hide();
    }



}
void playerWindow::on_Fold()
{
   	if(Player->status == true && Player->turn == true && gameStatus != -1)
    {
        to_dealer["event"] = "fold";
        to_dealer["chat"] = Player->playerName + " folded.";
		Player->status = false;
	
        chat_message msg;
        std::string t = to_dealer.dump();

        msg.body_length(t.size());
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);
    }
    else if(Player->status == false)
    {
        Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
        dialog.run();
        dialog.hide();

    }


    else if(gameStatus == -1)
    {
		Gtk::MessageDialog dialog(*this, "Game hasn't started yet.");
        dialog.run();
        dialog.hide();    
    
    }


    else if(Player->turn == false)
    {
    	Gtk::MessageDialog dialog(*this, "It is not your turn.");
        dialog.run();
        dialog.hide();
    }
}

void playerWindow::on_Check()
{
	if(curr_bet == 0 && Player->status == true && Player->turn == true && gameStatus != 2 && Player->checked == false && gameStatus != -1)
    {
        to_dealer["event"] = "check";
        to_dealer["chat"] = Player->playerName + " checked.";

        chat_message msg;
        std::string t = to_dealer.dump();

        msg.body_length(t.size());
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);
    }

    else if(Player->status == false)
    {
        Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
        dialog.run();
        dialog.hide();

    }
    else if(gameStatus == -1)
    {
		Gtk::MessageDialog dialog(*this, "Game hasn't started yet.");
        dialog.run();
        dialog.hide();    
    
    }
    else if(Player->turn == false)
    {
    	Gtk::MessageDialog dialog(*this, "It is not your turn.");
        dialog.run();
        dialog.hide();
    }
    else if(gameStatus == 2)
    {
    	Gtk::MessageDialog dialog(*this, "It is a swapping round.");
        dialog.run();
        dialog.hide();
    
    }
    else if(curr_bet != 0)
    {
        Gtk::MessageDialog dialog(*this, "Cannot check when already bet.");
        dialog.run();
        dialog.hide();
    }
    
    else if(Player->checked == true)
    {	
    	Gtk::MessageDialog dialog(*this, "You have already checked.");
        dialog.run();
        dialog.hide();
    }
}


void playerWindow::on_Bet()
{
    if(curr_bet == 0 && Player->status == true && Player->turn == true && gameStatus != 2 && gameStatus != -1)
    {
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
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);
    }
    else if(Player->status == false)
    {
        Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
        dialog.run();
        dialog.hide();

    }
    else if(gameStatus == -1)
    {
		Gtk::MessageDialog dialog(*this, "Game hasn't started yet.");
        dialog.run();
        dialog.hide();    
    
    }
    else if(Player->turn == false)
    {
    	Gtk::MessageDialog dialog(*this, "It is not your turn.");
        dialog.run();
        dialog.hide();
    }
    else if(gameStatus == 2)
    {
    	Gtk::MessageDialog dialog(*this, "It is a swapping round.");
        dialog.run();
        dialog.hide();
    
    }
    else if(curr_bet != 0)
    {
        Gtk::MessageDialog dialog(*this, "Call current bet or raise.");
        dialog.run();
        dialog.hide();
    }

    
}

void playerWindow::on_CardSwap()
{
    if(gameStatus != 2)
    {
        Gtk::MessageDialog dialog(*this, "Not a swapping round.");
        dialog.run();
        dialog.hide();
    }
    
    else
    {	
    	if(Player->turn == false)
    	{	
    		Gtk::MessageDialog dialog(*this, "It is not your turn.");
    		dialog.run();
    		dialog.hide();	
    	}
    	
        else if(curr_player->swapped)
        {
            Gtk::MessageDialog dialog(*this, "Already swapped.");
            dialog.run();
            dialog.hide();
        }
        else
        {
            int swapCount = 0;
            string tempSwap = "";
            if(swap1.get_active())
            {
                swapCount++;
                tempSwap = tempSwap + "1";
            }
            if(swap2.get_active())
            {
                swapCount++;
                tempSwap = tempSwap + "2";
            }
            if(swap3.get_active())
            {
                swapCount++;
                tempSwap = tempSwap + "3";
            }
            if(swap4.get_active())
            {
                swapCount++;
                tempSwap = tempSwap + "4";
            }
            if(swap5.get_active())
            {
                swapCount++;
                tempSwap = tempSwap + "5";
            }
            if(swapCount > 3)
            {
                Gtk::MessageDialog dialog(*this, "Cannot do more than three swaps.");
                dialog.run();
                dialog.hide();
            }
            else
            {
                int count = 0;
                if(swapCount == 0)
                {
                    tempSwap = "0";
                }
                else
                {
                    count = tempSwap.length();
                }
                curr_player->swapped = true;
                to_dealer["event"] = "swap";
                to_dealer["swap_cards"] = tempSwap;
                to_dealer["chat"] = Player->playerName + " swapped " + std::to_string(count) + " card/s.";

                swap1.set_active(false);
                swap2.set_active(false);
                swap3.set_active(false);
                swap4.set_active(false);
                swap5.set_active(false);


                chat_message msg;
                std::string t = to_dealer.dump();

                msg.body_length(t.size());
                std::memcpy(msg.body(), t.c_str(), msg.body_length());
                msg.encode_header();
                c->write(msg);
            }

        }
    }
}

void playerWindow::on_Exit()
{
	
   	if(Player->status == true && Player->turn == true && gameStatus != -1)
    {
        to_dealer["event"] = "fold";
        to_dealer["chat"] = Player->playerName + " exited.";
		Player->status = false;
	
        chat_message msg;
        std::string t = to_dealer.dump();

        msg.body_length(t.size());
        std::memcpy(msg.body(), t.c_str(), msg.body_length());
        msg.encode_header();
        c->write(msg);
        hide();
    }
    else if(Player->status == false)
    {
        Gtk::MessageDialog dialog(*this, "You haven't put ante yet.");
        dialog.run();
        dialog.hide();

    }


    else if(gameStatus == -1)
    {
		Gtk::MessageDialog dialog(*this, "Game hasn't started yet.");
        dialog.run();
        dialog.hide();    
    
    }


    else if(Player->turn == false)
    {
    	Gtk::MessageDialog dialog(*this, "It is not your turn.");
        dialog.run();
        dialog.hide();
    }
}



void playerWindow::on_Send()
{

    to_dealer["event"] = "chat";
    to_dealer["chat"] =  Player->playerName + ": " + std::string((Chat.get_text()).c_str());

    Chat.set_text("");

    std::string t = to_dealer.dump();

    chat_message msg;

    msg.body_length(t.size());
    std::memcpy(msg.body(), t.c_str(), msg.body_length());
    msg.encode_header();
    c->write(msg);

}



void playerWindow::on_chip_1()
{
    if(Player->chip1 <= 0)
    {
        Gtk::MessageDialog dialog(*this, "Not enough $1 chips.");
        dialog.run();
        dialog.hide();
    }
    else
    {
        Player->chip1--;
        temp1++;
        Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

        balanceLabel->set_markup("Balance: " + to_string(Player->balance) +
                                 "\t$1: " + to_string(Player->chip1) +
                                 "\t$5: " + to_string(Player->chip5) +
                                 "\t$25: " + to_string(Player->chip25));

        std::string your_bet_s = Amount.get_text();
        int your_bet = std::atoi(your_bet_s.c_str());
        your_bet = your_bet + 1;
        Amount.set_markup("<b>" + to_string(your_bet) + "</b>");
    }


}
void playerWindow::on_chip_5()
{
    if(Player->chip5 <= 0)
    {
        Gtk::MessageDialog dialog(*this, "Not enough $5 chips.");
        dialog.run();
        dialog.hide();
    }
    else
    {
        Player->chip5--;
        temp5++;
        Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

        balanceLabel->set_markup("Balance: " + to_string(Player->balance) +
                                 "\t$1: " + to_string(Player->chip1) +
                                 "\t$5: " + to_string(Player->chip5) +
                                 "\t$25: " + to_string(Player->chip25));

        std::string your_bet_s = Amount.get_text();
        int your_bet = std::atoi(your_bet_s.c_str());
        your_bet = your_bet + 5;
        Amount.set_markup("<b>" + to_string(your_bet) + "</b>");
    }

}
void playerWindow::on_chip_25()
{
    if(Player->chip25 <= 0)
    {
        Gtk::MessageDialog dialog(*this, "Not enough $25 chips.");
        dialog.run();
        dialog.hide();
    }
    else
    {
        Player->chip25--;
        temp25++;
        Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

        balanceLabel->set_markup("Balance: " + to_string(Player->balance) +
                                 "\t$1: " + to_string(Player->chip1) +
                                 "\t$5: " + to_string(Player->chip5) +
                                 "\t$25: " + to_string(Player->chip25));

        std::string your_bet_s = Amount.get_text();
        int your_bet = std::atoi(your_bet_s.c_str());
        your_bet = your_bet + 25;
        Amount.set_markup("<b>" + to_string(your_bet) + "</b>");
    }

}

void playerWindow::on_Clear()
{
    Player->chip1 = Player->chip1 + temp1;
    Player->chip5 = Player->chip5 + temp5;
    Player->chip25 = Player->chip25 + temp25;

    temp1 = 0;
    temp5 = 0;
    temp25 = 0;

    Player->balance = Player->chip1 * 1 + Player->chip5 * 5 + Player->chip25 * 25;

    balanceLabel->set_markup("Balance: " + to_string(Player->balance) +
                             "\t$1: " + to_string(Player->chip1) +
                             "\t$5: " + to_string(Player->chip5) +
                             "\t$25: " + to_string(Player->chip25));



    Amount.set_markup("<b>0</b>");
}

//gtk classes end


int main(int argc, char* argv[])
{
    for(int i = 0; i < 5; i++)
    {
        chatBox[i] = "";
    }


    curr_bet = 0;
    to_dealer["total_bet"] = 0;

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
        std::thread t([&io_context]()
        {
            io_context.run();
        });

        Gtk::Main app(argc, argv);
        playerNameWindow w;
        Gtk::Main::run(w);

        c->close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

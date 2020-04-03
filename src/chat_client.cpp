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
#include "asio.hpp"
#include "chat_message.hpp"
#include <gtkmm.h>
#include "game.hpp"

using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

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
	set_position(Gtk::WIN_POS_CENTER_ALWAYS);
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


	player p(playerName, true);

	playerWindow w(p);
	Gtk::Main::run(w);
} // get player's name from entry, opens playerWindow


playerWindow::playerWindow(player p):Player(p){

	set_title(Player.playerName + "'s Game Window");
	set_border_width(30);
	resize(600,400);
	set_position(Gtk::WIN_POS_CENTER_ALWAYS);
	Box.set_spacing(10);

	cardsBox.set_spacing(10);
	Card1.set("blank_card.jpg");
	cardsBox.pack_start(Card1);

	Card2.set("blank_card.jpg");
	cardsBox.pack_start(Card2);

	Card3.set("blank_card.jpg");
	cardsBox.pack_start(Card3);

	Card4.set("blank_card.jpg");
	cardsBox.pack_start(Card4);

	Card5.set("blank_card.jpg");
	cardsBox.pack_start(Card5);

	Box.pack_start(cardsBox);

	Amount.set_placeholder_text("eg: 5");
	Box.pack_start(Amount);

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

    Exit.set_label("Exit");
    Exit.signal_clicked().connect(sigc::mem_fun(*this, &playerWindow::on_Exit));
    Box.pack_start(Exit);


	Box.show_all();
	add(Box);
}

playerWindow::~playerWindow() {}

void playerWindow::on_Menu() {}
void playerWindow::on_Help() {}
void playerWindow::on_Call() {}
void playerWindow::on_Raise() {}
void playerWindow::on_Fold() {}
void playerWindow::on_Bet() {}
void playerWindow::on_Exit() {
	hide();
}


void playerWindow::on_Send(){
	chatMessage = Chat.get_text();

	char line[chat_message::max_body_length + 1];

	strcpy(line, chatMessage.c_str());
	
	chat_message msg;
	msg.body_length (strlen(line));
	std::memcpy(msg.body(), line, msg.body_length());
	msg.encode_header();

	assert(c);
	c->write(msg);
}

void playerWindow::on_CardSwap() {}
void playerWindow::on_SitOut() {}
void playerWindow::on_chip_1() {}
void playerWindow::on_chip_5() {}
void playerWindow::on_chip_25() {}

//gtk classes end


int main(int argc, char* argv[])
{
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

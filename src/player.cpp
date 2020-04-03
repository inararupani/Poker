#include <iostream>
#include <vector>
#include <string>
#include "game.hpp"
#include "chat_message.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

//Card Class Functions
Card::Card () {
}

Card::Card(int value, SUIT suit) {
	this->value = value;
	this->suit = suit;
}

int Card::compareCard(Card secondCard) {
	int diff = 0;
	diff = this->value - secondCard.value;
	return diff;
}


//Class Deck Functions
Deck::Deck() {
	for(int j = DIAMONDS; j <= SPADES; j++){
        for(int k = 1; k < 14; k++){
            deckOfCards.push_back(Card(k,(SUIT)j));
        }
    }
}


void Deck::shuffle_deck() {
	random_shuffle(deckOfCards.begin(), deckOfCards.end()); 
}

Card Deck::get_card() {
	Card retCard = deckOfCards.at(0);
	deckOfCards.erase(deckOfCards.begin());
	return retCard;
}

//Class hand Functions
hand::hand() {

}

void hand::addCard(Card card) {
	if(handOfCards.size() < 5){
		handOfCards.push_back(card);
	}
}

void hand::discardCard(int index) { // index = 0 is the first card in the vector
	if(handOfCards.size() > 0 && index < handOfCards.size()){
		handOfCards.erase(handOfCards.begin()+(index-1));	
	}
}

void hand::sequenceHand() { // puts hand in sequence based on card value, should have five cards
	Card temp;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < (5-i-1) ; j++){
			if(handOfCards.at(j).value > handOfCards.at(j+1).value){
				temp = handOfCards.at(j);
				handOfCards.at(j) = handOfCards.at(j+1);
				handOfCards.at(j+1) = temp;
			}
		}
	}
}


int hand::compareHand(hand firstHand, hand secondHand) {
// difficult
}

vector <Card> hand::getHand(){
	return handOfCards;
}

//Class player Functions
//player::player() {}
//player::player(string PlayerName, int StartBalance, int id, bool status) {}
player::player(string playerName, bool status) {
	this->playerName = playerName;
	this->status = status;
	id = boost::uuids::random_generator()();
	balance = 100;
}
void player::update_hand(Card newCard) {}
int player::request_card_swap() {}
void player::placeBet(int amount) {}
void player::check() {}
void player::call() {}
void player::sitOut() {}
void player::fold() {}
void player::raise(int amount) {}
string player::playerMessage() {}

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
	this->chatMessage = Chat.get_text();

	/*chat_message msg;
	msg.body_length(std::strlen(chatMessage));
	std::memcpy(msg.body(), chatMessage, msg.body_length());
	msg.encode_header();
	c.write(msg);
	*/	
}

void playerWindow::on_CardSwap() {}
void playerWindow::on_SitOut() {}
void playerWindow::on_chip_1() {}
void playerWindow::on_chip_5() {}
void playerWindow::on_chip_25() {}

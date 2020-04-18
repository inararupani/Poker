#include <iostream>
#include <vector>
#include <string>
#include "game.hpp"
#include "chat_message.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
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

string Card::generateCardName(){
	string suit;
	switch(this->suit){
		case 0: suit = "D"; break;
		case 1: suit = "H"; break;
		case 2: suit = "C"; break;
		case 3: suit = "S"; break;
		default: std::cerr << "Invalid card suit." << std::endl;
	}
	string name = std::to_string(this->value) + "" + suit;
	return name;
}


//Class Deck Functions
Deck::Deck() {
	for(int j = D; j <= S; j++){
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
	if(handOfCards.size() > 0 && index < (int)handOfCards.size()){
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
	return 0;
}

vector <Card> hand::getHand(){
	return handOfCards;
}

//Class player Functions
//player::player() {}
//player::player(string PlayerName, int StartBalance, int id, bool status) {}
player::player(string playerName) {
	this->playerName = playerName;
	this->status = false;
	id = boost::uuids::to_string(boost::uuids::random_generator()());
	balance = 100;
	chip1 = 25;
	chip5 = 10;
	chip25 = 1;
}
void player::update_hand(Card newCard) {}
int player::request_card_swap() {
	return 0;
}
void player::placeBet(int amount) {}
void player::check() {}
void player::call() {}
void player::sitOut() {}
void player::fold() {}
void player::raise(int amount) {}
string player::playerMessage() {
	return "0";
}


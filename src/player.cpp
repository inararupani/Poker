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
	chip1 = 25;
	chip5 = 10;
	chip25 = 1;
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


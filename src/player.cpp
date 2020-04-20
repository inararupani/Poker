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

int Card::getValue(){
    
    return (value);
}

SUIT Card::getSuit(){
    
    return (suit);
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



bool hand::isPair(){

    
    if(handOfCards.at(0).getValue() == handOfCards.at(1).getValue() || handOfCards.at(1).getValue()==handOfCards.at(2).getValue() || handOfCards.at(2).getValue()==handOfCards.at(3).getValue() ||
       handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    else
        return false;
}

bool hand::isTwoPair(){

    
    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue())
        return true;
    else if(handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    else if (handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    else
        return false;
}

bool hand::isThreeOfAKind(){

    
    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(1).getValue()==handOfCards.at(2).getValue()) 
        return true;
    else if(handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue())
        return true;
    else if(handOfCards.at(2).getValue()==handOfCards.at(3).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue()) 
        return true;
    else
        return false;
}

bool hand::isFlush()
{

    
    for (int count=0; count<4; count++)
    {
      if(handOfCards.at(count).getSuit()!=handOfCards.at(count+1).getSuit()){
      	return false;
      } 
    }
    return true;
}

bool hand::isStraight(){

    for(int count=0; count<4; count++)
    {
        if((handOfCards.at(count).getValue()-1) != handOfCards.at(count+1).getValue()){
            return false;
       	}
    }
    
    return true;
}

bool hand::isFullHouse(){

    
    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue()) 
        return true;
    else if(handOfCards.at(2).getValue()==handOfCards.at(3).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue() && handOfCards.at(0).getValue()==handOfCards.at(1).getValue())
        return true;
    else
        return false;
}


bool hand::isFourOfAKind(){
	    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue())
	    	return true;
	    else if (handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
	    	return true;
		else
			return false;
}


bool hand::isRoyalFlush(){
    	
	for (int count=0; count<4; count++)
    {
      if(handOfCards.at(count).getSuit()!=handOfCards.at(count+1).getSuit()){
      	return false;
      }
    }   
    

    if(handOfCards.at(0).getValue() == 1 && handOfCards.at(1).getValue() == 10 && handOfCards.at(2).getValue() == 11 && handOfCards.at(3).getValue() == 12 && handOfCards.at(4).getValue() == 13){
    	return true;
    }
    
    return false;    
   
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
	swapped = false;
	turn = false;
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

string getRank(hand H1){
	if(H1.isRoyalFlush())
		return "a Royal Flush";
	else if(H1.isStraight() && H1.isFlush())
		return "a Straight Flush";
	else if(H1.isFourOfAKind())
		return "Four of a kind";
	else if(H1.isFullHouse())
		return "a Full House";
	else if(H1.isFlush())
		return "a Flush";
	else if(H1.isStraight())
		return "a Straight";
	else if(H1.isThreeOfAKind())
		return "three of a kind";
	else if(H1.isTwoPair())
		return "two pairs";
	else if(H1.isPair())
		return "a pair";
	else
		return "a high card";


}

int compareHand(hand H1, hand H2){
	int score1 = 0;
	int score2 = 0;
	
	
	if(H1.isRoyalFlush())
		score1 = 10;
	else if(H1.isStraight() && H1.isFlush())
		score1 = 9;
	else if(H1.isFourOfAKind())
		score1 = 8;
	else if(H1.isFullHouse())
		score1 = 7;
	else if(H1.isFlush())
		score1 = 6;
	else if(H1.isStraight())
		score1 = 5;
	else if(H1.isThreeOfAKind())
		score1 = 4;
	else if(H1.isTwoPair())
		score1 = 3;
	else if(H1.isPair())
		score1 = 2;
	else
		score1 = 1;
		
	if(H2.isRoyalFlush())
		score2 = 10;
	else if(H2.isStraight() && H1.isFlush())
		score2 = 9;
	else if(H2.isFourOfAKind())
		score2 = 8;
	else if(H2.isFullHouse())
		score2 = 7;
	else if(H2.isFlush())
		score2 = 6;
	else if(H2.isStraight())
		score2 = 5;
	else if(H2.isThreeOfAKind())
		score2 = 4;
	else if(H2.isTwoPair())
		score2 = 3;
	else if(H2.isPair())
		score2 = 2;
	else
		score2 = 1;

	if(score1 != score2)
		return score1 - score2;
	
	
	if(H1.handOfCards.at(4).getValue() != H2.handOfCards.at(4).getValue())
		return H1.handOfCards.at(4).getValue() - H2.handOfCards.at(4).getValue();
		
	if(H1.handOfCards.at(3).getValue() != H2.handOfCards.at(3).getValue())
		return H1.handOfCards.at(3).getValue() - H2.handOfCards.at(3).getValue();
		
	if(H1.handOfCards.at(2).getValue() != H2.handOfCards.at(2).getValue())
		return H1.handOfCards.at(2).getValue() - H2.handOfCards.at(2).getValue();
		
	if(H1.handOfCards.at(1).getValue() != H2.handOfCards.at(1).getValue())
		return H1.handOfCards.at(1).getValue() - H2.handOfCards.at(1).getValue();
		
	if(H1.handOfCards.at(0).getValue() != H2.handOfCards.at(0).getValue())
		return H1.handOfCards.at(0).getValue() - H2.handOfCards.at(0).getValue();
	
	return 0;
}



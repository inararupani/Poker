#include <iostream>
#include <vector>
#include <string>
#include "deck.hpp"
#include <algorithm> 
#include <ctime>       
#include <cstdlib>

int myrandom (int i) { return std::rand()%i;}

Deck::Deck()
{
    for(int j = D; j <= S; j++)
    {
        for(int k = 1; k < 14; k++)
        {
            deckOfCards.push_back(Card(k,(SUIT)j));
        }
    }
}


void Deck::shuffle_deck()
{
	std::srand ( unsigned ( std::time(0) ) );
    std::random_shuffle(deckOfCards.begin(), deckOfCards.end(), myrandom);
}

Card Deck::get_card()
{
    Card retCard = deckOfCards.at(0);
    deckOfCards.erase(deckOfCards.begin());
    return retCard;
}

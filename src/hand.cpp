#include <iostream>
#include <vector>
#include <string>

#include "hand.hpp"

hand::hand()
{

}

void hand::sequenceHand()   // puts hand in sequence based on card value, should have five cards
{
    Card temp;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < (5-i-1) ; j++)
        {
            if(handOfCards.at(j).value > handOfCards.at(j+1).value)
            {
                temp = handOfCards.at(j);
                handOfCards.at(j) = handOfCards.at(j+1);
                handOfCards.at(j+1) = temp;
            }
        }
    }
}



bool hand::isPair()
{


    if(handOfCards.at(0).getValue() == handOfCards.at(1).getValue() || handOfCards.at(1).getValue()==handOfCards.at(2).getValue() || handOfCards.at(2).getValue()==handOfCards.at(3).getValue() ||
            handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    return false;
}

int hand::pairVal()
{
    if(handOfCards.at(0).getValue() == handOfCards.at(1).getValue())
    	return handOfCards.at(0).getValue(); 
    if(handOfCards.at(1).getValue() == handOfCards.at(2).getValue())
     	return handOfCards.at(1).getValue(); 
    if(handOfCards.at(2).getValue() == handOfCards.at(3).getValue())
    	return handOfCards.at(2).getValue(); 
    if(handOfCards.at(3).getValue() == handOfCards.at(4).getValue())
    	return handOfCards.at(3).getValue(); 
    return 0;
}

bool hand::isTwoPair()
{


    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue())
        return true;
    if(handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    if (handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    return false;
}

int hand::twoPairHigh()
{	
    return handOfCards.at(3).getValue();

}
int hand::twoPairLow()
{
	return handOfCards.at(1).getValue();
}

bool hand::isThreeOfAKind()
{


    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(1).getValue()==handOfCards.at(2).getValue())
        return true;
    if(handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue())
        return true;
    if(handOfCards.at(2).getValue()==handOfCards.at(3).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    return false;
}

int hand::threeOfAKindVal()
{
	return handOfCards.at(2).getValue();
}

bool hand::isFlush()
{


    for (int count=0; count<4; count++)
    {
        if(handOfCards.at(count).getSuit()!=handOfCards.at(count+1).getSuit())
        {
            return false;
        }
    }
    return true;
}

bool hand::isStraight()
{

    for(int count=0; count<4; count++)
    {
        if((handOfCards.at(count).getValue()-1) != handOfCards.at(count+1).getValue())
        {
            return false;
        }
    }

    return true;
}

bool hand::isFullHouse()
{


    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    if(handOfCards.at(2).getValue()==handOfCards.at(3).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue() && handOfCards.at(0).getValue()==handOfCards.at(1).getValue())
        return true;
    return false;
}

bool hand::isFourOfAKind()
{
    if(handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue())
        return true;
    else if (handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(2).getValue()==handOfCards.at(3).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue())
        return true;
    else
        return false;
}

int hand::fourOfAKindVal()
{
	return handOfCards.at(1).getValue();
}

bool hand::isRoyalFlush()
{

    for (int count=0; count<4; count++)
    {
        if(handOfCards.at(count).getSuit()!=handOfCards.at(count+1).getSuit())
        {
            return false;
        }
    }


    if(handOfCards.at(0).getValue() == 1 && handOfCards.at(1).getValue() == 10 && handOfCards.at(2).getValue() == 11 && handOfCards.at(3).getValue() == 12 && handOfCards.at(4).getValue() == 13)
    {
        return true;
    }

    return false;

}


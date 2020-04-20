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

    
    for (int count=0; count<3; count++)
    {
      if(handOfCards.at(count).getSuit()!=handOfCards.at(count+1).getSuit()){
      	return false;
      } 
    }
    return true;
}

bool hand::isStraight(){

    for(int count=0; count<3; count++)
    {
        if((handOfCards.at(count).getValue()-1) != handOfCards.at(count+1).getValue()){
            return false;
       	}
    }
    
    return true;
}

bool hand::isFullHouse(){

    
    if((handOfCards.at(0).getValue()==handOfCards.at(1).getValue() && handOfCards.at(1).getValue()==handOfCards.at(2).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue()) 
        return true;
    else if((handOfCards.at(2).getValue()==handOfCards.at(3).getValue() && handOfCards.at(3).getValue()==handOfCards.at(4).getValue() && handOfCards.at(0).getValue()==handOfCards.at(1).getValue())
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
    	
	for (int count=0; count<3; count++)
    {
      if(handOfCards.at(count).getSuit()!=handOfCards.at(count+1).getSuit()){
      	return false;
      }
    }   
    

    if(handOfCards.at(0).getValue() == 1 && handOfCards.at(1) == 10 && handOfCards.at(2) == 11 && handOfCards.at(3) == 12 && handOfCards.at(4) == 13){
    	return true;
    }
    
    return false;    
   
}

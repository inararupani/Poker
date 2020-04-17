#ifndef game_h
#define game_h
#include <gtkmm.h>
#include <iostream>
#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace std;

typedef enum {D,H,C,S} SUIT;
class Card
{
public:
    int value;
    SUIT suit;

    Card();
    Card(int value, SUIT suit);
    int compareCard(Card secondCard); // compare two cards to put cards in sequence for players
    string generateCardName();

};

class Deck{
public:
    vector <Card> deckOfCards;

    Deck(); // initialize deck of 52 cards

    void shuffle_deck(); // shuffles the vector of initially 52 cards using using pre-defined function random_shuffle()
    Card get_card(); //gets the card needs to be distributed or swapped during the game

};

class hand{
public:
    vector <Card> handOfCards;

    hand(); // initailly set all five cards to NULL, before dealer deals

    void addCard(Card card);
    void discardCard(int index); //returns the index of the card number needs to be discard to be swapped
    void sequenceHand();//puts the hand in sequence
    int compareHand(hand firstHand, hand secondHand); // compare two hands to decide winner for dealer and returns which holds higher priority
    vector <Card> getHand();
};


class player{
public:
    string id;
    int balance;
    int chip1;
    int chip5;
    int chip25;

    string playerName;
    hand playerHand;
    bool status;

    //constructors
    //player();
    player(string PlayerName);
    //player(string PlayerName, bool status); //Used for spectators

    int get_id();
    int get_balance();
    bool get_status();
    //number of $1 chips
    int set_chip1();
    int get_chip1();
    //number of $5 chips
    int set_chip5();
    int get_chip5();
    //number of $25 chips
    int set_chip25();
    int get_chip25();
    hand get_playerHand(); //gets the player hands

    void update_hand(Card newCard); // updates hand when receives card from the dealer
    int request_card_swap(); // returns number of cards to be swapped, discards unwanted cards from hand

    void placeBet(int amount);
    void check();
    void call();
    void sitOut(); //automatically folds the next round (next round the player will be sitting outside)
    void fold();
    void raise(int amount);
    string playerMessage();

};

class dealer{
public:
    int playerCount;
    Deck deck;
    vector <player> players;
    int activePlayer;
    int roundNum;
    int potAmount;
    bool GameOn; //0- False, 1- true - determines if the game is on or not.

    dealer();
    virtual ~dealer();

    void get_gameOn(); //gets the status of game
    void set_gameOn(); //sets the status of game.
    int get_activePlayer(); //get number of active players
    void StartGame();
    void set_pot(int potAmount);
    int get_pot(int potAmount);
    int get_totalBalance(player p); //gets how much balance a player have
    int set_totalBalance(player p); //update the amount distributed to a player after winning a round
    void deal();
    void swap(int swapCount);
    int generateID(player p);
    void updatePot(int amount);
    player winner();
    void awardPot();
    string dealerMessage();
};


class playerNameWindow: public Gtk::Window{
public:
    string playerName;

    playerNameWindow();
    virtual ~playerNameWindow();

protected:
    void on_OK(); // get player's name from entry, opens playerWindow

    Gtk::Label nameLabel;
    Gtk::Entry Name;
    Gtk::Button OK;
    Gtk::VBox Box;
};

class playerWindow: public Gtk::Window{
public:
    int temp1, temp5, temp25;
    player *Player;
    string chatMessage;

    playerWindow(player *p);
    virtual ~playerWindow();

    //Signal handlers
    void on_Start();
    void on_Ante();
    void on_Menu();
    void on_Help();
    void on_Bet();
    void on_Call();
    void on_Raise();
    void on_Fold();
    void on_Exit();
    void on_CardSwap();
    void on_Send();
    void on_SitOut();
    void on_chip_1();
    void on_chip_5();
    void on_chip_25();
    void on_Clear();



    Gtk::Image DealerAvatar;
    Gtk::Image PlayerAvatar;
    Gtk::Image chips;
    Gtk::Image deck;
    Gtk::Label* Card1;
    Gtk::Label* Card2;
    Gtk::Label* Card3;
    Gtk::Label* Card4;
    Gtk::Label* Card5;
    Gtk::Label Amount; // amount to bet or raise
    Gtk::Entry Chat;
    Gtk::Label* chatLabel;
    Gtk::Label* currBetLabel;
    Gtk::Label* totPotLabel; 
    Gtk::Label currentBet;
    Gtk::Label totalPot;
    Gtk::Label balanceLabel;
    Gtk::Label yourBet;
    Gtk::Label chatHistory;
    Gtk::Button Start;
    Gtk::Button Ante;
    Gtk::Button chip1;
    Gtk::Button chip5;
    Gtk::Button chip25;
    Gtk::Button clear;
    Gtk::Button Help;
    Gtk::Button Bet;
    Gtk::Button Call;
    Gtk::Button Raise;
    Gtk::Button Fold;
    Gtk::Button Exit;
    Gtk::Button CardSwap;
    Gtk::Button SitOut;
    Gtk::Button Send;
    Gtk::VBox Box;
    Gtk::HBox startBox;
    Gtk::HBox chipBox;
    Gtk::HBox actionBox;
    Gtk::HBox cardsBox;

    //Gtk::Grid grid;//Keeps the object in a certain way
};

#endif

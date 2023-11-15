#ifndef SOLITAIRE_H
#define SOLITAIRE_H

#include "Hand.h"
#include "Deck.h"
#include <iostream>

class Solitaire
{
public:
    Solitaire();
    ~Solitaire();
    void dealGame();
    int  cycleDeck();
    void playGame();
    void moveToColumn(int destinationCol, int col, int slot, bool lastUnflipedCard);
    bool checkCanMove(Card* c, int col, int row, bool lastCard, bool lastUnflippedCard);
    void printField();
    //struct cardState {Card* pCard; bool faceUp;};
    //cardState sColumn[7][19];
    //cardState sAces[4][13];
    //cardState sSolitaireDeck[26];
    //cardState sDrawPile[26];
    //struct gameSaveNode {cardState sColumn[7][19]; cardState sAces[4][13];cardState sSolitaireDeck[26];cardState sDrawPile[26];gameSaveNode* next;};

    typedef struct CardState {      
        Card* pCard;
        bool faceUp; 
        int slot; 
    }    CardState;

    /********************  slot decoding  ***********************
    Aces:    C = 0-12   S = 13-25   H = 26-38   D = 39-51
    Columns 1(52-70)    2(71-89)   3(90-108)    4(109-127)  5(128-146)  6(147-165)  7(166-184)
    Decks   Base Deck(185-210)  Draw Pile(211-236) 
    *************************************************************/  

    typedef struct GameNode {
        CardState gameState[52];    // A node consists of 52 CardStates, one for each card in the deck
        struct GameNode* next;      // Pointer to the next game state in the linked list
    } GameNode;

    GameNode* head = NULL;          // Head of the linked list
    void  saveGameState();
    void  loadGameState();
    void  printNode(GameNode* node);
    void  clearLinkedList(GameNode*& head);
    
    Hand  getColumn(int col);
    int   getTopAce(int suit);
    Card* getColCardAt(int col, int row);
    int   getColumnSize(int col);
    Card* getTopDrawPileCard();
    Deck* getDrawPile();
    Deck* getDeck();
    int   getDeckSize();
    int   getDrawPileFlipped();
    void  flipThreeDP();
    Hand* getAceStack(int suit);
    int   getDrawPileSize();
    Card* getDrawPileAt(int slot);
    int   getMoves();
    Card* removeColCard(int col, int row, bool lastCard);
    Card* removeForAce(int col, int row);
    void aceStackMove(int col, int row, int suit, Card* pCard, bool lastCard);
    void playFromDrawPile(int col);
    void playFromAces(int col, int suit);
    void reuseDeck();
    bool getWin();
    bool checkAutoFinish();
    bool nextCard();
    

private:
    Deck solitaireDeck;
    Deck drawPile;

    Hand cardCol[7];
    Hand suit[4];
    Hand Aces[4]; // = {clubs, spades, hearts, diamonds};

    std::vector<int> possibleMoves;

    bool aceFlag = false;
    int  lastCardClicked;
    int  dpSize;
    int  dpFlipUp;
    int  cardCycle;
    bool win;
    int  moves;
};
#endif // SOLITAIRE_H
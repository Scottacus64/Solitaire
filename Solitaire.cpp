#include "Solitaire.h"
#include <array>
#include <vector>

using namespace std;


Solitaire::Solitaire()
{
    playGame();
} 


Solitaire::~Solitaire()
{

}


void Solitaire::playGame()
{
    solitaireDeck = Deck();
    solitaireDeck.shuffle();
    clearLinkedList(head);
}


void Solitaire::dealGame()
{
    for (int i=0; i<4; i++) {Aces[i].clearHand();}
    for (int i=0; i<7; i++) {cardCol[i].clearHand();}
    for (int row=0; row<7; row++)
    {
        for (int col=0; col<7; col++)
        {
            if (col == row)                         // if it is the last card in a cardCol
            {
                Card* pCard = solitaireDeck.deal();
                int id = pCard->getID();
                pCard->flipCard();                       // flip the card over to show its top side
                cardCol[col].addCard(pCard);
            }
            else if (col > row)                     // if it is not the last card in a cardCol
            {
                Card* pCard = solitaireDeck.deal();
                cardCol[col].addCard(pCard);
            }
        }
    }   
    drawPile = Deck(0);
    dpSize = 0;
    win = false;
    moves = 0;
    printField();
}


Hand Solitaire::getColumn(int col)
{
    return cardCol[col];
}


Card* Solitaire::getColCardAt(int col, int row)
{
    Card* c = cardCol[col].getCard(row);
    return c;
}


Deck* Solitaire::getDeck()
{
    Deck* p_deck = &solitaireDeck;
    return p_deck;
}


void Solitaire::printField()
 {
    Card* cardArray[7];
    for (int row=0; row<19; row++)
    {
        bool emptyRow = true;
        for (int col=0; col<7; col++)
        {
            if (row < cardCol[col].getSize())
            {
                Card* pCard = cardCol[col].getCard(row);
                cardArray[col]= pCard;
                emptyRow = false;
            }
            else
            {
                cardArray[col] = nullptr;
            }
        }
        if (emptyRow == false)
        {
            for (int i=0; i<7; i++)
            {
                Card* pCard = cardArray[i];
                if (pCard)
                {
                    pCard->printCard();
                }

                cout << "\t";
            }
            cout << "\n";
        }
    }
    saveGameState();
 }


Card* Solitaire::getTopDrawPileCard()
{
    cout << "GTCDP deck = " << &drawPile << endl;;
    Card* pCard = drawPile.getTopDeckCard();
    drawPile.printTopCard();
    cout << "***** GTCDP Card Address = " << pCard << "\n\n";
    int id = pCard->getID();
    cout << "****** GTDPC id = " << id << endl;;
    return pCard;
}


Deck* Solitaire::getDrawPile()
{
    Deck* p_drawPile = &drawPile;
    return p_drawPile;
}


int Solitaire::getDrawPileSize()
{
    int dpSize = drawPile.cardsLeft();
    std::cout << "getDrawPileSize " << dpSize << "\n";
    return dpSize;
}


int Solitaire::getDrawPileFlipped()
{
    return dpFlipUp;
}


int Solitaire::getDeckSize()
{
    int size = solitaireDeck.cardsLeft();
    return size;
}


Card* Solitaire::getDrawPileAt(int slot)
{
    Card* m_pc = drawPile.getDeckCardAt(slot);
    return m_pc;
}


int Solitaire::getColumnSize(int col)
{
    int size = cardCol[col].getSize();
    return size;
}



int Solitaire::getTopAce(int suit)
{
    int id = Aces[suit].getLastCardID();
    return id;
}


Hand* Solitaire::getAceStack(int suit)
{
    Hand* aceStack = &Aces[suit];
    return aceStack;
}


bool Solitaire::getWin()
{
    return win;
}


int Solitaire::cycleDeck()
{
    if (solitaireDeck.cardsLeft() == 0 && getDrawPileSize() > 0){reuseDeck();}
    int cardsDelt = 0;
    cout << "the drawPile address is: " << &drawPile << endl;;
    int dpSize = drawPile.cardsLeft();
    if (dpSize > 0)                             // this flips all draw pile cards face down so that later we can see which
    {                                           // cards are playable (ie face up)
        for (int i=0; i<dpSize; i++) 
        {
            std::cout << "flipping draw pile card\n";
            Card* c = drawPile.getDeckCardAt(i);
            c->setFaceUp(false);
        }
    }
    dpFlipUp = 0;
    for (int i=0; i<3; i++)
    {
        std::cout << "i = " << i;
        if (solitaireDeck.cardsLeft() > 0)
        {
            Card* pCard = solitaireDeck.deal();          // get and remove the Card object from the solitaireDeck
            pCard->flipFaceUp();                         // set it's face up bool to true
            int id = pCard->getID();

            cout << "\nthe card delt's id is " << id << endl;;
            cout << "\nthe card delt face up is " << pCard->getFaceUp() << endl;;
            cout << "the delt cards address is " << pCard << endl;;
            drawPile.addCard(pCard);                    // add the Card object to the deck drawPile
            dpFlipUp ++;                            // this shows how many cards in the dp are flipped up and ready to play
            cardsDelt ++;
            pCard->printCard();
            cout << " ######## draw pile size: " << dpSize << endl;;
        }
        Card* pCard = drawPile.getTopDeckCard();
        int id = pCard->getID();
        cout << "Card Cycle id of top card: " << id << endl;;
    }
    
    cout << "\033[0m" << "\ntop card: ";
    
    drawPile.printTopCard();
    cout << "\033[0m" <<endl;;
        
    int remaining = solitaireDeck.cardsLeft();
    cout << "Cycle deck cards remaining = " << remaining << "\n\n";
    if (remaining == 0){bool win = checkAutoFinish();}
    if (win == true){std::cout << "WIN!!!";}
    moves ++;
    return cardsDelt;                               // this let's the calling function know how many cards were delt
}



/*******************************************************************************************
 *          This is the setion that looks over the ID number and suit of a card sent to it         
 *          to check if that card can be played on either a column or the aces above         
********************************************************************************************/
bool Solitaire::checkCanMove(Card* pCard, int col, int row, bool lastCard, bool lastUnflippedCard)
{
    bool cardRed;
    bool columnRed;
    bool sameCardClicked = false;
    bool canMove = false;
    char suits[4] = {'C', 'S', 'H', 'D'};
    int moveSize;
    bool aceMatch = false;

    possibleMoves.clear();
    cout << "CheckCanMove's Card address is: " << pCard << endl;;
    bool fu = pCard->getFaceUp();
    cout << "the card in checkCanMove's facuUp value is: "  << fu << endl;;

    if (pCard->getFaceUp() == true)                   // if the card is not flipped up, disregard it
    {
        int id = pCard->getID();                      // from the card's ID, face value and suit
        cout << "CheckPosMoves id = " << id << endl;;

        /********  check if the same card was clicked again  *******/
        if (id == lastCardClicked)                  // if the same card is clicked again
        {
            sameCardClicked = true;                 // if the same card is clicked again      
            cardCycle ++;                           // increment the counter
            moveSize = possibleMoves.size();
            if (cardCycle >= moveSize)              // if the counter has reached the possible spots for the card
            {
                cardCycle = 0;                      // reset the counter to zero
                aceFlag = false;                    // if the card can go to an aceStack reset so it can go back up there
            }
        }
        else 
        {
            cardCycle = 0;                          // otherwise a new card has been clicked so reset the counter
            aceFlag = false;                        // reset the ace flag as well
        }           

        possibleMoves.clear();                      // clear out possible moves now that we've passed the checks
        lastCardClicked = id;                       // set an id to check next click to see if the same card is clicked

        int cardValue = pCard->getFaceValue();
        char suit = pCard->getSuit();

        cout << "IN CheckPossMoves CardValue = " << cardValue << " suit = " << suit << endl;;

        /************ check each of the four aceStacks at the top to see if the card can move here ***********/
        for (int j=0; j<4; j++)                     // check if a card can play on an ace stack at the top of the table
        {
            // if the array is not empty, a card has not been moved to aces and it is the last card in the cardCol
            cout << "aceStack" << j << " size = " << Aces[j].getSize()<< " LC = " << lastCard << " AM = " << aceMatch << endl;;
            if (Aces[j].getSize() > 0 && lastCard == true && aceMatch == false)  
            {
                int aceID = Aces[j].getLastCardID();             // get the id of the last card in the Aces stack
                int aceStackValue = aceID%13;
                char aceSuit = Aces[j].getLastCardSuit();        // as well as its suit
                int cardID = id%13;
                if (cardID == 0) {cardID = 13;}
                cout << "aceStack last card= " << aceStackValue << " incoming card = " << cardID << endl;;
                if (cardID == aceStackValue+1 && suit == aceSuit)      // if the clicked card's id is one more than the last one in the ace stack
                {
                    possibleMoves.push_back((j+1)*10);          // set it as a possible move 
                    aceMatch = true;
                }
            }
            else                                   // if the array is empty, the card is an ace and matches the slot suit
            {
                if (id % 13 == 1 && suit == suits[j] && aceMatch == false)  // if it is an Ace card in play
                {
                    possibleMoves.push_back((j+1)*10);  // use a multiple of ten to show this special situation  
                    aceMatch = true;                                        
                }
            }
        }  

        /********** Check each of the seven card columns to see if the card can move to one of these ***********/
        for (int i=0; i<7; i++)                         // go through each cardCol
        {
            int colID = cardCol[i].getLastCardID();     // get the last card in the column's ID
            int colVal = colID % 13;
            if (colVal == 0) {colVal = 13;}             // set Kings to 13
            char colSuit = cardCol[i].getLastCardSuit();

            if (colID < 27) {columnRed = false;}        // set cardCol card as red or black
            else {columnRed = true;}

            if (id < 27) {cardRed = false;}             // set the clicked card as red or black
            else {cardRed = true;}

            // if the card is one less in value than the last card in a cardCol and the color of the card is opposite

            if (colVal == cardValue+1 && columnRed != cardRed)    
            {
                possibleMoves.push_back(i);             // set this as a possible move
            }
        }

        /*********  check if the card is a king and there is an empty cardColumn  **********/
        for (int i=0; i<7; i++)
        {
            int size =cardCol[i].getSize();
            if (size == 0 && cardValue == 13)
            {
                possibleMoves.push_back(i);
            }
        }
    }

    moveSize = possibleMoves.size();
    if (moveSize>0) {canMove=true;}
    cout << "Possible moves: " ;
    for (int i=0; i<moveSize; i++)
    {
        cout << possibleMoves[i] << "/";
    }
    cout << "\n";

    /*****************  determine where the card will move to if it is possible for the card to move ****************/
    for (int slot=0; slot<moveSize; slot++)     // go through each possible move
    {
        cout << "slot = " << slot << " move size = " << moveSize << " CardCycle = " << cardCycle << endl;;
        int destination = possibleMoves[slot];
        cout << "destination = " << destination << endl;;
        bool aceMatch = false;                      // this prevents the ace from being added more than once

        /********* this checks if a card can move to an ace stack which is the highest priority move *********/
        if (destination > 9 && aceFlag == false)    
        {
            int suit = (destination/10)-1;          // calculate the suit from the x10 value     
            aceStackMove(col, row, suit, pCard, lastCard);    // move the card to the ace stack
            aceMatch = true;                        
        }
        /********* this checks for moving to a cardColumn **********/
        else
        {
            bool moved = false;   // this prevents more than one card to be added if more than one possible column
            if (aceMatch == false and moved == false)   // need to skip if the card has moved to an aceStack
            {
                if (slot == cardCycle)
                {
                    moveToColumn(destination, col, row, lastUnflippedCard); 
                    moved = true;
                }
            }             
        }
    }
    bool win = checkAutoFinish();
    if (win == true){std::cout << "WIN!!!";}
    return canMove;
}



/******************  this will move the card to a column in the play area  *******************/
void Solitaire::moveToColumn(int destinationCol, int col, int slot, bool lastUnflippedCard)
{
    cout << "In MoveToColumn\n";
    if (col < 100)                              // this means that it came from the play area and not aces or draw pile
    {
        int colSize = cardCol[col].getSize();
        cout << "row: " << slot << " col size: " << colSize << " last card: " << lastUnflippedCard << endl;;
        for (int i= slot; i<colSize; i++)                    // take each card from the selected card to the end
        {
            std:: cout << "ROW: " << i << endl;;
            Card* pCard = cardCol[col].removeCard(slot);                  // remove it from the sorce and..
            cardCol[destinationCol].addCard(pCard);                      // add it to the destination
        }
        int colLength = cardCol[col].getSize();
        cout << "after moving cards colLength = " << colLength << endl;;
        if (colLength > 0)                                          // if there are still cards in the col, flip one
        {
            Card* t = cardCol[col].getCard(colLength-1);
            if (lastUnflippedCard == true) {t->flipFaceUp();}
        }
    }
    else 
    {
        if (slot >99)           // this is coming from the drawPile
    {
        cout << "Draw Pile Card\n";
        playFromDrawPile(destinationCol);
        
    }
        else                    // this is coming from the aces
        {
            cout << "aceStack card\n";
            playFromAces(destinationCol, slot);
        }
    }
    moves ++;
}


void Solitaire::playFromDrawPile(int col)
 { 
    bool canMove;
    std::cout << "In playFromDrawPile Draw Pile Size = " << dpFlipUp << "\n"; 
    if(dpFlipUp > 0) 
    {      
        Card* pCard = drawPile.deal();
        cardCol[col].addCard(pCard);
        dpFlipUp --;
        int dpSize = drawPile.cardsLeft();
        std::cout << "dpFlipUp = " << dpFlipUp << " dpSize = " << dpSize << "\n\n"; 
        if (dpFlipUp == 0 && dpSize >0){flipThreeDP();}
    }
}


void Solitaire::flipThreeDP()
{
    int dpSize = drawPile.cardsLeft();
    if (dpSize > 0)
    std::cout << "In flip three\n\n";
    {
        for (int i=dpSize-1; i>dpSize-4; i--)
        {
            if (i>-1)
            {
                dpFlipUp ++;
                Card* c = drawPile.getDeckCardAt(i);
                c->setFaceUp(true);
            }
        }
    }
}


void Solitaire::playFromAces(int col, int suit)
{
    cout << "In Play from Aces\n";
    int size = Aces[suit].getSize();
    Card* pCard = Aces[suit].removeCard(size-1);
    cardCol[col].addCard(pCard);
    moves ++;
}


void Solitaire::aceStackMove(int col, int row, int suit, Card* pCard, bool lastCard)
{
    cout << "In AceStackMove Suit = " << suit << " Col = " << col << " Row = " << row << endl;;
    cout << "Incomming card is: ";
    pCard->printCard();
    cout << "\n";

    Card* pTmpCard;
    if (col<100)
    {      
        pTmpCard = removeForAce(col, row);                     // remove the card from the cardCol
    }
    else
    {
        pTmpCard = drawPile.deal();
        dpFlipUp --;
        if (dpFlipUp == 0){flipThreeDP();}
    }
    Aces[suit].addCard(pTmpCard);                              // move the card to the Ace stack
    aceFlag = true;

    win = true;                                         // this checks for the win condition
    for (int i=0; i<4; i++)                                   
    {
        int aceSize = Aces[i].getSize();
        if (aceSize < 13) {win = false;}
    }
    cout << "win = " << win << " Out of AceStackMove\n";
    moves ++;
}



Card* Solitaire::removeColCard(int col, int row, bool lastCard)
{
    Card* pCard;
    int length = getColumnSize(col);
        cout << "** RemoveColCard row = " << row << " length = " << length << endl;;
    for (int slot=row; slot<length; slot++)             // remove each card from the column
    {
        pCard = cardCol[col].removeCard(slot);
    }

    if (row > 0 && lastCard == true)
    {
        Card* pTempCard = cardCol[col].getCard(row-1);
        pTempCard->flipFaceUp();
    }
    return pCard;
 }


 Card* Solitaire::removeForAce(int col,int row)
 {
    Card* pCard = cardCol[col].removeCard(row);
    if (row > 0)
    {
        Card* pTempCard = cardCol[col].getCard(row-1);
        pTempCard->flipFaceUp();
    }
    return pCard;
 }


 void Solitaire::reuseDeck()
 {
    int pileSize = drawPile.cardsLeft();
    for (int i=0; i<pileSize; i++)
    {
        Card* pCard = drawPile.deal();
        dpSize --;
        pCard->setFaceUp(false);
        solitaireDeck.addCard(pCard);
    }
 }

 bool Solitaire::checkAutoFinish()
 {
    bool autoFinish = true;
    for (int i=0; i<7; i++)
    {
        for (int j=0; j<cardCol[i].getSize(); j++)
        {
            Card* pTestCard = cardCol[i].getCard(j);
            Card testCard = *pTestCard;
            if (testCard.getFaceUp() == false){autoFinish = false;}
        }
    }
    if (getDeckSize() > 0) {autoFinish = false;}
    if (getDrawPileSize() > 0) {autoFinish = false;}
    return autoFinish;
 }

 bool Solitaire::nextCard()
 {
    int totalCards = 0;
    int lowestColumn;
    for (int i=0; i<7; i++) {totalCards = totalCards + cardCol[i].getSize();}
    if (totalCards > 0)
    {
        int cardValues[7];
        for (int i=0; i<7; i++){cardValues[i] = cardCol[i].getLastCardValue();}
        int lowestValue = 20;
        for (int i=0; i<7; i++)
        {
            if (cardValues[i] > 0)
            {
                if (cardValues[i] < lowestValue)
                {
                    lowestValue = cardValues[i];
                    lowestColumn = i;
                } 
            }
        }
        int row = cardCol[lowestColumn].getSize() - 1;
        Card* pCard = cardCol[lowestColumn].removeCard(row);
        int ID = pCard->getID();
        int suit = ID/14;
        Aces[suit].addCard(pCard);
        return false;
    }
    else
    {return true;}
 
 }


int Solitaire::getMoves()
{
    return moves;
}

/************************************************************/
void Solitaire::printNode(Solitaire::GameNode* node)
{
    std::cout << "\n\n     Node pointer: " << node << "\n";
    for (int i=0; i<52;  i++)
    {
        Card* pCard = node->gameState[i].pCard;
        std::cout << "Pointer: " << pCard << " *****************\n";
        std::cout << "Card " << i << ":" << pCard->getID() <<"\n";
        std::cout << "faceUp: " << node->gameState[i].faceUp << "\n";
        int cSlot = node->gameState[i].slot;
        std::cout << "location: " << cSlot << "\n";
        if (cSlot<52)
            {std::cout << "Aces Suit: " << cSlot/13 << " slot: " << cSlot%13 << "\n";}
        else if (cSlot < 185)
            {std::cout << "Column Number " << (cSlot-52)/19 << " slot: " << (cSlot-52)%19 << "\n";}
        else if (cSlot < 211)
            {std::cout << "Deal Deck Slot: " << (cSlot-185)%26 << "\n";}
        else
            {std::cout << "Draw Pile Slot: " << (cSlot-211)%26 << "\n";}
    }
    std::cout << "**************************************************************************\n";
}

void Solitaire::saveGameState()
{
    Solitaire::GameNode* newNode = new Solitaire::GameNode; // make a GameNode* called newNode
    int cardSlot;
    cardSlot = 0;
    for (int i=0; i<4; i++)                             // for each of the Ace stacks
    {
        for (int j=0; j<13; j++)    
        {
            int aceLen = Aces[i].getSize();             // see if there are any cards there
            if (j < aceLen)
            {
                Card* pCard = Aces[i].getCard(j);       // get the Card objecrt
                if (pCard)
                {
                    CardState sCard;
                    sCard.pCard = pCard;                // set the struct's pointer, faceUp and locations
                    sCard.faceUp = pCard->getFaceUp();
                    sCard.slot = j + (i * 13);
                    newNode->gameState[cardSlot] = sCard;   // put the struct in the node being built
                    cardSlot ++;
                }
            }
        }
    }

    for (int i=0; i<7; i++)                             // next check each card column 
    {
        for (int j=0; j<19; j++)
        {
            int colLen = cardCol[i].getSize();
            if (j < colLen)                             // if the column has a card in this slot then...
            {
                Card* pCard = cardCol[i].getCard(j);
                if (pCard)
                {
                    CardState sCard;
                    sCard.pCard = pCard;                
                    sCard.faceUp = pCard->getFaceUp();
                    sCard.slot = 52 + j + (i*19);       
                    newNode->gameState[cardSlot] = sCard;
                    cardSlot ++;
                }
            }
        }
    }  

    int sdLen = solitaireDeck.cardsLeft();              // check the Deal Deck called solitaireDeck
    for (int i=0; i<26; i++)
    {
        if (i < sdLen)
        {
            Card* pCard = solitaireDeck.getDeckCardAt(i);
            if (pCard)
            {
                CardState sCard;
                sCard.pCard = pCard;
                sCard.faceUp = pCard->getFaceUp();
                sCard.slot = 185 + i;
                newNode->gameState[cardSlot] = sCard;
                cardSlot ++;
            }
        }
    }

    int dLen = drawPile.cardsLeft();                    // last check the Draw Pile
    for (int i=0; i<26; i++)
    {
        if (i < dLen)
        {
            Card* pCard = drawPile.getDeckCardAt(i);
            if (pCard)
            {
                CardState sCard;
                sCard.pCard = pCard;
                sCard.faceUp = pCard->getFaceUp();
                sCard.slot = 211 + i;
                newNode->gameState[cardSlot] = sCard;
                cardSlot ++;
            }
        }
    }
    newNode->next = head;                           // assign the current head to newNode's next value
    head = newNode;                                 // make newNode the Head
    std::cout << "**** PRINTING AFTER SAVE ****";
    printNode(newNode);
}


void Solitaire::loadGameState()
{
    if (head)                                           // Check if there's at least one saved game state.
    {  
        Solitaire::GameNode* previousNode = head;       // this makes a node previousNode that points to the current head
        if (head->next != nullptr)                      // make sure you are not at the very first node
        {
            head = head->next;                           // this pushes head back one node in the list
            std::cout << "**** PRINTING AFTER LOAD ****\n";

            printNode(head);                                    // print what we think is the head
            for (int i=0; i<4; i++){Aces[i].clearHand();}       // clear the hands and Decks
            for (int i=0; i<7; i++){cardCol[i].clearHand();}
            solitaireDeck.eraseDeck();
            drawPile.eraseDeck();
            for (int i=0; i<52; i++)                            // go through all 52 cards in the game
            {
                Card* pCard = head->gameState[i].pCard;              // get the Card
                pCard->setFaceUp(head->gameState[i].faceUp);     // its faceUp value
                int cSlot = head->gameState[i].slot;            // where it is located       
                if (cSlot<52)                                                   // check if it's in the Aces stacks
                    {
                        Aces[cSlot/13].addCardAt(pCard, cSlot%13);
                    }
                else if (cSlot < 185)                                           // or card calumns
                    {
                        cardCol[(cSlot-52)/19].addCardAt(pCard, (cSlot-52)%19);
                    }
                else if (cSlot < 211)                                           // or Dealing Deck
                    {   
                        solitaireDeck.addCardAt(pCard, (cSlot-185)%26);
                    }
                else                                                            // or Draw Pile
                    {
                        drawPile.addCardAt(pCard, (cSlot-211)%26);
                    }
            }
        }
        else
        {
            std::cout << "No other node\n";
        }
    }    
    else 
    {
        std::cout << "No Head\n";
    }
}


void Solitaire::clearLinkedList(Solitaire::GameNode*& head) 
{
    while (head) 
    {
        GameNode* temp = head;
        head = head->next;
        delete temp;
    }
}
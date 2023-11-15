#include "SolitaireUI.h"
#include <string>
#include <iostream>
#include <QPainter>
#include <QTimer>
#include <QFont>
#include <QPalette>
#include <QCoreApplication>
#include <QDir>

SolitaireUI::SolitaireUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SolitaireUI)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString assetPath = QDir::cleanPath(appDir + QDir::separator() + "pngs") + QDir::separator();

    ui->setupUi(this);

    green = QPixmap(assetPath + "green.png");
    // set up all of the card image QPixmaps
    cardImage[0] = QPixmap(assetPath + "0B.png");
    char suits[4] = {'C', 'S', 'H', 'D'};
    int slot = 0;
    for (int s=0; s<4; s++)
    {
        for (int i=1; i<14; i++)
        {
            slot++;
            cardImage[slot] = QPixmap(assetPath + QString::number(i) + suits[s]+ ".png");
        }
    }

    // set up the four Ace piles at the top
    for (int i = 0; i < 4; i++) 
    {
        std::string name = "A" + std::to_string(i);
        m_pA[i] = new QPushButton(QString::fromStdString(name), this);
        m_pA[i]->setObjectName(QString::fromStdString(name));
        m_pA[i]->setGeometry(QRect(290 + (i * 110), 0, 100, 140));
        m_pA[i]->setIcon(QPixmap()); 
        m_pA[i]->setText(QString());
        m_pA[i]->setEnabled(false);
        QSize iconSize(100, 140);
        m_pA[i]->setIconSize(iconSize);
        connect(m_pA[i], &QPushButton::clicked, this, &SolitaireUI::cardClicked);
    }

    // set up all of the columns of cards in the playfield area
    for (int k=0; k<7; k++) 
    {
        for (int j=0; j<19; j++) 
        {
            std::string name = "C" + std::to_string(j+(k*19));
            m_pC[j+(k*19)] = new QPushButton (QString::fromStdString(name), this);
            m_pC[j+(k*19)]->setObjectName(QString::fromStdString(name));
            m_pC[j+(k*19)]->setGeometry(QRect(130+(k*110), 160+(j*25), 100, 140));
            m_pC[j+(k*19)]->setIcon(QPixmap(QString::fromUtf8("../../VSC/CPP/Solitaire/CardPNGs/13S.png")));
            m_pC[j+(k*19)]->setText(QString());
            m_pC[j+(k*19)]->setStyleSheet(QString::fromUtf8("border: none"));
            m_pC[j+(k*19)]->setEnabled(false);
            QSize iconSize(100, 140);
            m_pC[j+(k*19)]->setIconSize(iconSize);
            connect(m_pC[j+(k*19)], &QPushButton::clicked, this, &SolitaireUI::cardClicked);
        }
    }

        // set up the deck and draw piles at the bottom
    int j=0;
    int k=10;    
    for (int i = 0; i < 4; i++) 
    {
        if (i>0){j=110;}
        std::string name = "D" + std::to_string(i);
        m_pD[i] = new QPushButton(QString::fromStdString(name), this);
        m_pD[i]->setObjectName(QString::fromStdString(name));
        m_pD[i]->setGeometry(QRect(290+j+k, 750, 100, 140));
        m_pD[i]->setText(QString());
        k+=20;
        QSize iconSize(100, 140);
        m_pD[i]->setIconSize(iconSize);
        connect(m_pD[i], &QPushButton::clicked, this, &SolitaireUI::cardClicked);
    }
    m_pD[0]->setIcon(QPixmap(cardImage[0]));
    m_pD[0]->setEnabled(true); 
    for(int i=1; i<4; i++)
    {
        m_pD[i]->setIcon(QPixmap()); 
        m_pD[i]->setEnabled(false);
        m_pD[i]->hide();
    }

    std::string name = "NewGame";
    m_newGame = new QPushButton(QString::fromStdString(name), this);
    m_newGame->setObjectName(QString::fromStdString(name));
    m_newGame->setGeometry(QRect(600, 740, 140, 50));
    m_newGame->setText(QString("New Game?"));
    connect(m_newGame, &QPushButton::clicked, this, &SolitaireUI::cardClicked);


    m_undo = new QPushButton("undo", this);
    m_undo->setObjectName("undo");
    m_undo->setGeometry(QRect(600, 800, 140, 50));
    m_undo->setText(QString("Undo"));
    connect(m_undo, &QPushButton::clicked, this, &SolitaireUI::undoPressed);


    QFont font;
    font.setPointSize(28);
    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::white);

    m_moves = new QLabel("moves", this);
    m_moves->setGeometry(QRect(600,830,200,100));
    m_moves->setFont(font);
    m_moves->setPalette(palette);
    m_moves->setText(QString("Moves = "));

    m_timer = new QLabel("Time: ", this);           
    m_timer->setGeometry(QRect(600, 860, 300, 100));
    m_timer->setFont(font);
    m_timer->setPalette(palette);
    //elapsedTimer.start();

    QObject::connect(&timer, &QTimer::timeout, [&]() {
        elapsedMilliseconds = elapsedTimer.elapsed();
        qint64 elapsedSeconds = elapsedMilliseconds / 1000;
        QString elapsedTimeStr = QString("Time: %1 seconds").arg(elapsedSeconds);
        m_timer->setText(elapsedTimeStr);
    });

    //timer.start(1000);

    m_pSolitaire = new Solitaire();

    dealCards();
}


SolitaireUI::~SolitaireUI()
{
    delete ui;
}


void SolitaireUI::dealCards()
{
    m_pSolitaire->dealGame();
    refreshScreen();
    m_pD[0]->setEnabled(true);
    m_pD[0]->setIcon(QPixmap(cardImage[0]));
    m_pD[0]->setText(QString());
    for (int i=1; i<4; i++)
    {
        m_pD[i]->setEnabled(false);
        m_pD[i]->setIcon(QPixmap());
        m_pD[i]->setText(QString());
        m_pD[i]->hide();
    }
    gameStarted = false;
    timer.stop();
    elapsedMilliseconds = 0;
    m_timer->setText("Time:");
}


void SolitaireUI::refreshScreen()
{
    for (int i=0; i<133; i++) 
    {
        m_pC[i]->setEnabled(false); 
        m_pC[i]->setIcon(QPixmap());
    }

    for (int col=0; col<7; col++)
    {
        Hand column = m_pSolitaire->getColumn(col);
        for (int card=0; card<column.getSize(); card++)
        {
            Card* pCard = column.getCard(card);
            int id = pCard->getID();              // id is the same as the cardImage value
            int pbCard = (col * 19) + card;     // there are 19 cards in a column
            m_pC[pbCard]->setEnabled(true);     // turn on the QPushButton enabled so it can be clicked
            m_pC[pbCard]->raise();              // raise the card above the hidden cards to prevent them from...
            if (pCard->getFaceUp() == true)       // intercepting mouse clicks
            {
                m_pC[pbCard]->setIcon(QPixmap(cardImage[id]));
            }
            else
            {
                m_pC[pbCard]->setIcon(QPixmap(cardImage[0]));
            }
        }
    }

    for (int i=0; i<4; i++)
    {
        int id = m_pSolitaire->getTopAce(i);
        if (id > 0)
        {
            m_pA[i]->setIcon(QPixmap(cardImage[id]));
            m_pA[i]->setEnabled(true);
        }
        else
        {
            m_pA[i]->setIcon(QPixmap());
            m_pA[i]->setEnabled(false);
        }
    }
    int moves = m_pSolitaire->getMoves();
    m_moves->setText(QString("Moves = ") + QString::number(moves));
}


void SolitaireUI::cardClicked()
{
    QPushButton* clickedCard = qobject_cast<QPushButton*>(sender());
    if (clickedCard) 
    {
        if (gameStarted == false){elapsedTimer.start(); timer.start(1000);}
        gameStarted = true;
        int dpSize = m_pSolitaire->getDrawPileSize();           // this allows expansion of the last draw piles to register
        if (dpSize > 2 && drawPileFlag == true ){cardsDelt = 3;}

        std::cout << "** Top of card clicked cardsDelt = " << cardsDelt << endl;;
        bool lastCard = false;
        bool lastUnflippedCard = false;
        QString cardName = clickedCard->objectName();
        QChar firstChar = cardName.at(0);
        QChar secondChar = cardName.at(1);
        if (firstChar == 'C')
        {
            cardName.remove(0, 1);                                              //strip the leading C
            std::string card = cardName.toStdString();
            int slot = std::stoi(card);
            int col = slot/19;
            int row = slot%19;
            Card* pCard = m_pSolitaire->getColCardAt(col, row);                   //find the solitaire card at this location
            int cardID = pCard->getID();                                             //get the card's ID
            int colLastCardID = m_pSolitaire->getColumn(col).getLastCardID();   //get the ID of the last card in the column
            if (cardID == colLastCardID) {lastCard = true;}                     //if they are equal then we have found the last card
            if (row > 0)
            {
                Card* p_test = m_pSolitaire->getColCardAt(col, row-1);          //this is used if a stack is moved and the last card
                bool flipped = p_test->getFaceUp();                             //in the column is not flipped
                if (flipped == false) {lastUnflippedCard = true;}
            }
            drawPileFlag = false;
            m_pSolitaire->checkCanMove(pCard, col, row, lastCard, lastUnflippedCard);
        }
        /*********** if the aceStack is clicked **************/
        else if(firstChar == 'A')
        {
            std::cout << "Clicked an A column\n";
            int suit = secondChar.digitValue();
            Hand* p_aceStack = m_pSolitaire->getAceStack(suit);
            Hand aceStack = *p_aceStack;
            int size = aceStack.getSize();
            
            Card* pCard = aceStack.getCard(size-1);
            drawPileFlag = false;
            m_pSolitaire->checkCanMove(pCard, 100, suit, true, false);
        }
        /*********** if the draw pile or deck stack is clicked *************/
        else if(firstChar == 'D')
        {
            for (int i=0; i<4; i++) {m_pD[i]->show();}
            std::string deck = cardName.toStdString();
            std::cout << "Clicked on " << deck << endl;;
            if (deck == "D0") 
            {
                cardsDelt = m_pSolitaire->cycleDeck();                              // deals cards and returns how many
                if (cardsDelt > 0) {updateDecks(0, 1); updateDecks(1,cardsDelt);}   // sets up the cards in draw piles
                Deck* p_deck = m_pSolitaire->getDeck();
                int dSize = p_deck->cardsLeft();
                drawPileFlag = false;
                if (dSize == 0) {updateDecks(0,0);}                                 // if the deck is empty update that data
            }
            else 
            {
                int dPiles = secondChar.digitValue();               // get the number of the pile clicked
                /*int dpSize = m_pSolitaire->getDrawPileSize();       // get the size of the draw pile
                
                if (dpSize < 3) {cardsDelt = dpSize;}               // if there are less than 3 cards in dp set cardsDelt to that number
                std::cout << "**** CardClicked cardsDelt = " << cardsDelt << " dpSize = " << dpSize << endl;;*/
                cardsDelt = m_pSolitaire->getDrawPileFlipped();
                if (dPiles == cardsDelt)                            // if the pile clicked is the active pile
                {
                    if (dpSize > 0) 
                    {
                        Card* pCard = m_pSolitaire->getTopDrawPileCard();
                        bool canMove = m_pSolitaire->checkCanMove(pCard, 100, 100, true, false);
                        std::cout << "CanMove = " << canMove << "\n";
                        if (canMove==true) {updateDecks(1,m_pSolitaire->getDrawPileFlipped());}    // needs a fresh dpSize
                    }
                    else 
                    {
                        updateDecks(1,0);
                        std::cout << "Draw Pile empty\n";
                    }
                }
            }
        }
        else            // must be in newGame
        {
            std::cout << "New Game\n";
            m_pSolitaire->playGame();
            dealCards();
        }
        checkForWin();
        refreshScreen();
        bool aFinish = m_pSolitaire->checkAutoFinish();
        if (aFinish == true){autoFinish();}
        m_pSolitaire->printField();
    }
}


void SolitaireUI::autoFinish()
{
    std::cout << "In autoFinish \n";
    bool gameDone = false;
    while (gameDone == false)
    {
        gameDone = m_pSolitaire->nextCard();
        refreshScreen();
        delayTimer(200);
    }
}





void SolitaireUI::checkForWin()
{
    bool win = m_pSolitaire->getWin();
    std::cout << "Win = " << win << endl;;
    if (win == true)
    {
        m_pD[0]->setEnabled(false);
        m_pD[0]->setText("WIN!!");
        m_pD[1]->setEnabled(false);
        m_pD[1]->setIcon(QPixmap()); 
        m_pD[1]->setText("WIN!!");
    } 
}


void SolitaireUI::enableDrawPile(int pile, int id)
{
    m_pD[pile]->setEnabled(true);
    m_pD[pile]->setIcon(QPixmap(cardImage[id]));
    m_pD[pile]->raise();
}


void SolitaireUI::disableDrawPile(int pile)
{
    m_pD[pile]->setEnabled(false);
    m_pD[pile]->setIcon(QPixmap());
    m_pD[pile]->setStyleSheet(QString::fromUtf8("border: none"));
    if (pile>1)
    {
        m_pD[pile-1]->setEnabled(true);
        m_pD[pile]->lower();              // lower the card 
    }
}

void SolitaireUI::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Paint the green background image
    painter.drawPixmap(0, 0, width(), height(), green);

    // ... your existing painting code
}


void SolitaireUI::delayTimer(int delay)
{
    QTimer timer;
    timer.setInterval(delay); 
    timer.start();
    // Use a loop to wait until the QTimer times out (25ms)
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    loop.exec();
}

void SolitaireUI::undoPressed()
{
    std::cout << "Undo button pressed\n";
    m_pSolitaire->loadGameState();
    refreshScreen(); 
    refreshDecks();
}


void SolitaireUI::updateDecks(int deck, int cDelt)
{
    if (deck == 0)
    {
        if (cDelt > 0)
        {
            m_pD[0]->setEnabled(true);
            m_pD[0]->setIcon(QPixmap(cardImage[0]));
            m_pD[0]->setText(QString());
        }
        else
        {
            m_pD[0]->setIcon(QPixmap());
            m_pD[0]->setText("Again?");  
        }
    }
    else
    {
    if (cDelt > 0)
        {
            for (int i=1; i<4; i++){disableDrawPile(i);}            // disable each draw pile to clear everything
            int dpSize = m_pSolitaire->getDrawPileSize();           // get the size of the drawPile
            int dpFaceUp = m_pSolitaire->getDrawPileFlipped();
            std::cout << "draw pile flipped cards = " << dpFaceUp << "\n";
            std::cout << "** UpdateDecks cards delt = " << cDelt << " dpSize = " << dpSize <<endl;;
            for (int i=1; i<dpFaceUp+1; i++)                           // go from D1 to D3
            {
                int slot = dpSize-1-dpFaceUp+i;                        // start with the -2 to 0 slot from the back
                std::cout << " slot = "<< slot<< endl;;
                Card* pCard = m_pSolitaire->getDrawPileAt(slot);   // get the Card pointer for this slot
                int id = pCard->getID();                           // get the card's id
                enableDrawPile(i, id);                              // enable that draw pile deck stack
                std::cout << "enable draw pile " << i << " *****\n";
            }
            drawPileFlag = true;
            std::cout << "**** UpdateDecks dpSize = " << dpSize << " cardsDelt = " << cardsDelt << endl;;
        }
        else
        {
            m_pD[1]->setEnabled(false);
            m_pD[1]->setText(QString());
            m_pD[1]->setIcon(QPixmap()); 
        }        
    }
}


void SolitaireUI::refreshDecks()
{
    int deckSize = m_pSolitaire->getDeckSize();
    if (deckSize > 0)                                       // This is the main solitaire deck
    {
        m_pD[0]->setEnabled(true);
        m_pD[0]->setIcon(QPixmap(cardImage[0]));
        m_pD[0]->setText(QString());
    }
    else
    {
        m_pD[0]->setIcon(QPixmap());
        m_pD[0]->setText("Again?");  
    }
    int dpSize = m_pSolitaire->getDrawPileSize();           // this is for the draw pile and the flipped cards
    int dpfSize = m_pSolitaire->getDrawPileFlipped();
    std::cout << "DP size = " << dpSize << "\n";
    std::cout << "Flipped DP = " << dpfSize << "\n";
    for (int i=1; i<4; i++){disableDrawPile(i);}            // disable each draw pile to clear everything
    if (dpSize > 0)                                         // if there are cards in the draw pile
    { 
        int pileSlot = 1;
        for (int i=0; i<dpfSize; i++)                       // cycle through any flipped cards as they are active
        {
            Card* pC = m_pSolitaire->getDrawPileAt(dpSize-1-i); // the piles from left to right are populated from the top of the deck
            std::cout << i <<  ":" << pC->getFaceUp() <<"\n";
            if (pC->getFaceUp() == true)                        // down so start at the last element of the draw pile and work down the pile
            {
                enableDrawPile(pileSlot,pC->getID());
            }
            pileSlot ++;
        }
    }
    else
    {
        std::cout << "Disable Draw Piles\n" ;
       for (int i=1; i<4; i++) {disableDrawPile(i);} 
    }
}
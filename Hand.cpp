#include "Hand.h"
#include <iostream>

Hand::Hand()
{
}


Hand::~Hand()
{  
}


Card* Hand::removeCard(int position)
{
    std::cout << "** H remove card, pos = " << position << "\n";
    int size = m_hand.size();
    std::cout <<"Hand size is: " << size << "\n";
    Card* pCard = m_hand[position];
    m_hand.erase(m_hand.begin() + position);
    return pCard;
}


void Hand::addCard(Card* pCard)
{
    m_hand.push_back(pCard);
}


void Hand::addCardAt(Card* pCard, int slot)
{
    m_hand.insert(m_hand.begin() + slot, pCard);
}


Card* Hand::getCard(int position)
{
    Card* pCard = m_hand[position];
    return pCard;
}


int Hand::getSize()
{
    int size =  m_hand.size();
    return size;
}



void Hand::printHand()
{
    for (auto pCard : m_hand)
    {
        pCard->printCard();
        std::cout << " ";
    }
}


void Hand::sortHandValue()
{
    std::sort(m_hand.begin(), m_hand.end(), compareByCardValue);
}


bool Hand::compareByCardValue(Card* pCard1, Card* pCard2)
{
    return pCard1->getFaceValue() < pCard2->getFaceValue();
}


void Hand::sortActualValue()
{
    std::sort(m_hand.begin(), m_hand.end(), compareByCardActualValue);
}

bool Hand::compareByCardActualValue(Card* pCard1, Card* pCard2)
{
    return pCard1->getActualValue() < pCard2->getActualValue();
}


void Hand::sortHandSuit()
{
    std::sort(m_hand.begin(), m_hand.end(), compareBySuit);
}


bool Hand::compareBySuit(Card* pCard1, Card* pCard2)
{
    return pCard1->getSuit() < pCard2->getSuit();
}


void Hand::sortHandSuitAndValue()
{
    std::sort(m_hand.begin(), m_hand.end(), compareBySuitAndCardValue);
}


bool Hand::compareBySuitAndCardValue(Card* pCard1, Card* pCard2)
{
    if (pCard1->getSuit() == pCard2->getSuit())
    {
        return pCard1->getFaceValue() < pCard2->getFaceValue();
    }
    return pCard1->getSuit() < pCard2->getSuit();
}


void Hand::clearHand()
{
    m_hand.clear();
}


int Hand::getLastCardID()
{
    int id = -2;
    if (m_hand.size() > 0)
    {
        id = m_hand[m_hand.size()-1]->getID();
    }
    return id;
}


int Hand::getLastCardValue()
{
    int value = 0;
    if (m_hand.size() > 0)
    {
        value = m_hand[m_hand.size()-1]->getFaceValue();
    }
    return value;
}


char Hand::getLastCardSuit()
{
    char suit;
    if (m_hand.size() > 0)
    {
        suit = m_hand[m_hand.size()-1]->getSuit();
    }
    return suit; 
}


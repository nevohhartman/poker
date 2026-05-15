#include "splashkit.h"
#include "splashkit-arrays.h"



enum suit_type
{
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES
};

enum rank_type
{
    TWO = 2 ,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 11,
    QUEEN = 12,
    KING = 13,
    ACE = 14
};
struct card
{
    suit_type suit;
    rank_type rank;
};



string card_to_display(const card &c)
{

    string suit_result;
    string rank_result;

    if (c.rank >=2 && c.rank <= 10)
    {
         rank_result = to_string(c.rank);
    }
    else
    {
        switch (c.rank)
        {
            case JACK:
                rank_result = "J";
                break;
            case QUEEN:
                rank_result = "Q";
                break;
            case KING:
                rank_result = "K";
                break;
            case ACE:
                rank_result = "A";
        }
    }

    switch (c.suit)
    {
        case CLUBS:
            suit_result = "D";
            break;
        case DIAMONDS:
            suit_result = "H";
            break;
        case HEARTS:
            suit_result = "C";
            break;
        case SPADES:
            suit_result = "S";
    }

    return suit_result + rank_result + ".png";
}


int main()
{
    card my_card;
    my_card.suit = HEARTS;
    my_card.rank = ACE;

    write_line(to_string(my_card.rank) + to_string(my_card.suit));

    return 0;
}
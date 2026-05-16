#include "splashkit.h"
#include "splashkit-arrays.h"

enum suit_type
{
    CLUBS = 0,
    DIAMONDS = 1,
    HEARTS = 2,
    SPADES = 3
};

enum rank_type
{
    TWO = 2,
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

enum hand_type
{
    HIGH_CARD = 0,
    PAIR = 1,
    TWO_PAIR = 2,
    THREE_OAK = 3,
    STRAIGHT = 4,
    FLUSH = 5,
    FULL_HOUSE = 6,
    FOUR_OAK = 7,
    STRAIGHT_FLUSH = 8
};

struct card
{
    suit_type suit;
    rank_type rank;
    bitmap image = nullptr;
    bool drawn = false;
};

struct player
{
    string name;
    bitmap avatar;
    fixed_array<card *, 2> hand;
    bool user;
    
    int chips;
};




class betting_display
{
private:
    const double fold_width = 133.33;
    const double fold_height = 60;
    const double fold_x = 266.66;
    const double fold_y = 800 - 100;
    // CHIPS DISPLAY
    const double chips_width = 1000 / 3;
    const double chips_height = 60;
    const double chips_x = fold_x + 1.25 * fold_width;
    const double chips_y = fold_y - chips_height - 5;
    //PLAYER CARDS
    double card_width = chips_width*0.35;
    double card_height = (334/240) * card_width;
    double first_card_x = chips_x + chips_width/10 - card_width/2 + 8;
    double first_card_y = chips_y - card_height - 80;
    double card_scale_factor = 0.5;
    double card_spacing = 12;
    //CENTRE CARDS
    double center_card_x = 1200/2 - card_width*2.5 - 2*card_spacing - 65;
    double center_card_y = 240 - card_height/2;

    // Fold
    const rectangle fold_button = {fold_x, fold_y, fold_width, fold_height};

    // Call
    const rectangle call_button = {fold_x + fold_width, fold_y, fold_width, fold_height};

    // Raise

    // Raise -
    const rectangle minus_button = {fold_x + 2 * fold_width, fold_y, fold_width / 2, fold_height};

    // Raise
    const rectangle raise_button = {fold_x + 2.5 * fold_width, fold_y, fold_width, fold_height};

    // Raise +
    const rectangle plus_button = {fold_x + 3.5 * fold_width, fold_y, fold_width / 2, fold_height};

    // ALL IN
    const rectangle all_in_button = {fold_x + 4 * fold_width, fold_y, fold_width, fold_height};

    // Chips Display
    const rectangle chips_display = {chips_x, chips_y, chips_width, chips_height};

    //PLAYER CARDS
    rectangle card1 = {first_card_x, first_card_y, card_width, card_height};
    rectangle card2 = {first_card_x + card_width + card_spacing, first_card_y, card_width, card_height};


    //CENTRE CARDS

    //FLOP (FIRST 3 CARDS)
    rectangle centre_card1 = {center_card_x, center_card_y, card_width, card_height};
    rectangle centre_card2 = {center_card_x + card_width + card_spacing, center_card_y, card_width, card_height};
    rectangle centre_card3 = {center_card_x + 2 * (card_width + card_spacing), center_card_y, card_width, card_height};
    //TURN (4TH CARD)
    rectangle centre_card4 = {center_card_x + 3 * (card_width + card_spacing), center_card_y, card_width, card_height};
    //RIVER (5TH CARD)
    rectangle centre_card5 = {center_card_x + 4 * (card_width + card_spacing), center_card_y, card_width, card_height};


    void draw_button(const rectangle &button, const color &button_color, const string &label)
    {
        fill_rectangle(button_color, button);

        int font_size = fold_height * 0.4;
        double tw = text_width(label, "Roboto", font_size);
        double th = text_height(label, "Roboto", font_size);

        draw_text(label, COLOR_WHITE, "Roboto", font_size,
                  button.x + (button.width - tw) / 2,
                  button.y + (button.height - th) / 2);
    }

public:
    betting_display()
    {
        load_font("Roboto", "Roboto.ttf");
        load_bitmap("card1", "SA.png");
        load_bitmap("card2", "H4.png");
    }

    void display(double move1 = 0, double move2 = 0)
    {
        centre_card1 = {center_card_x + move1, center_card_y + move2, card_width, card_height};
        centre_card2 = {center_card_x + card_width + card_spacing + move1, center_card_y + move2, card_width, card_height};
        centre_card3 = {center_card_x + 2 * (card_width + card_spacing) + move1, center_card_y + move2, card_width, card_height};
        draw_bitmap(bitmap_named("card1"), card1.x, card1.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        draw_bitmap(bitmap_named("card2"), card2.x, card2.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        draw_bitmap(bitmap_named("card1"), centre_card1.x, centre_card1.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        draw_bitmap(bitmap_named("card2"), centre_card2.x, centre_card2.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        draw_bitmap(bitmap_named("card1"), centre_card3.x, centre_card3.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        draw_bitmap(bitmap_named("card2"), centre_card4.x, centre_card4.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        draw_bitmap(bitmap_named("card1"), centre_card5.x, centre_card5.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        draw_button(fold_button, COLOR_RED, "Fold");
        draw_button(call_button, COLOR_BLUE, "Call");
        draw_button(minus_button, COLOR_GRAY, "-");
        draw_button(raise_button, COLOR_GREEN, "Raise");
        draw_button(plus_button, COLOR_GRAY, "+");
        draw_button(all_in_button, COLOR_ORANGE, "ALL IN");
        draw_button(chips_display, COLOR_BLACK, "Chips: 1000");
    }
};

class game
{
private:
    fixed_array<card, 52> deck;
    fixed_array<card *, 5> board_cards;
    fixed_array<player, 4> players;

    card *deal()
    {
        int index;

        do
        {
            index = rnd(52);
        } while (deck[index].drawn);

        deck[index].drawn = true;

        return &deck[index];
    }





public:
    game()
    {
        // FILL DECK
        for (int i = 0; i < 52; i++)
        {
            deck[i].suit = (suit_type)(i / 13);
            deck[i].rank = (rank_type)(i % 13 + 2);
        }
    }


    void reshuffle()
    {
        for (int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                players[i].hand[j]->drawn = false;
                players[i].hand[j] = nullptr;
            }
        }

        for(int i = 0; i < 5; i++)
        {
            board_cards[i]->drawn = false;
            board_cards[i] = nullptr;
        }
    }

    void deal_cards()
    {
        for (int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                players[i].hand[j] = deal();
            }
        }

        for (int i = 0; i < 5; i++)
        {
            board_cards[i] = deal();
        }
    }




    void insertion_sort(fixed_array<card*, 7> &cards)
    {
        for (int i = 1; i < 7; i++)
        {
            card* key = cards[i];

            int j = i -1;

            while (j >= 0 && cards[j]->rank > key->rank)
            {
                cards[j+1] = cards[j];
                j--;
            }
            cards[j+1] = key;
        }
    }



    ///HAND EVALUATION



    int flush(const int (&suit)[3], const fixed_array<card*, 7> &cards)
    {
        for(int i = 0; i < 3; i++)
        {
            if (suit[i] == 5)
            {
                for (int i = 6; i > 3; i--)
                {
                    if (cards[i]->suit == i)
                    {
                        return cards[i]->rank;
                    }
                }
            }
        }

        return -1;

    }




    int straight_flush(const fixed_array<card*, 7> &cards)
    {   

        int tally = 0;
        int high = 0;
        for (int i = 6; i > 0; i--)
        {
            if (tally == 0 && i == 3)
            {
                return -1;
            }

            if (tally == 5)
            {
                return high;
            }

            if (cards[i]->rank - 1 == cards[i-1]->rank && cards[i]->suit == cards[i-1]->suit)
            {
                if (tally == 0)
                {
                    high = cards[i]->rank;
                }
                tally ++;

            }
        }

    }

    int straight(const int (&freq)[15])
    {
        int tally = 0;
        int high = 0;
        for (int i = 14; i < 1; i--)
        {
            if (tally == 5)
            {
                return high;
            }

            if (tally == 0 && i == 4 )
            {
                return -1;
            }

            if (freq[i] >= 3)
            {
                return -1;
            }


            if(freq[i] > 0 && freq[i-1] > 0)
            {
                if (tally == 0)
                {
                    high = i;
                }
                tally ++;
            } 

        }

        return -1;
    }

    void hand_eval(player &player)
    {
        fixed_array<card*, 7> cards;



        //FILL CARDS
        for (int i = 0; i < 7; i++)
        {
            if(i < 5)
            {
                cards[i] = board_cards[i];
            }
            else
            {
                cards[i] = player.hand[i-5];
            }
        }

        //SORT THEM
        insertion_sort(cards);

        int frequency[15] = {0};
        int suit[3] = {0};


        for (int i = 0; i < 7; i++)
        {
            frequency[cards[i]->rank] ++;
            if(cards[i]->rank == 14)
            {
                frequency[1] ++;
            }

            suit[cards[i]->suit] ++;
        }
        


        //CHECK FLUSH
        

    }

};

class display_handler
{
    private:
        //#region Card Display
        string card_to_display(const suit_type &s, const rank_type &r)
        {

            string suit_result;
            string rank_result;

            if (r >= 2 && r <= 10)
            {
                rank_result = to_string(r);
            }
            else
            {
                switch (r)
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
                    break;
                default:
                    rank_result = "error";
                }
            }

            switch (s)
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
                break;
            default:
                suit_result = "error";
            }

            return suit_result + rank_result + ".png";
        }
        //#endregion
        void image_loader(player &p)
        {
            p.avatar = load_bitmap("test", "40157.png");
            p.hand[0]->image = load_bitmap("card1", card_to_display(p.hand[0]->suit, p.hand[0]->rank));
            p.hand[1]->image = load_bitmap("card2", card_to_display(p.hand[1]->suit, p.hand[1]->rank));
        }

        void player_display(player &p, double scale_factor, double x, double y)
        {



            image_loader(p);
            x = x - (260 * scale_factor) - 50;
            draw_bitmap(p.hand[0]->image, x, y + 210 * scale_factor, option_scale_bmp(scale_factor, scale_factor));
            draw_bitmap(p.hand[1]->image, x + 260 * scale_factor, y + 210 * scale_factor, option_scale_bmp(scale_factor, scale_factor));
        }



};







int main()
{
    player my_player;
    card card1;
    card card2;

    card1.suit = HEARTS;
    card1.rank = ACE;
    card2.suit = SPADES;
    card2.rank = KING;
    my_player.hand[0] = &card1;
    my_player.hand[1] = &card2;
    betting_display betting;
    open_window("poker", 1200, 800);
    int move1 = 0;
    int move2 = 0;
    int speed = 5;
    while (!quit_requested())
    {

        clear_screen(COLOR_GREEN);
        process_events();
        if(key_down(LEFT_KEY))
        {
            move1 -= speed;
        }
        if(key_down(RIGHT_KEY))
        {
            move1 += speed;
        }
        if(key_down(UP_KEY))
        {
            move2 -= speed;
        }
        if(key_down(DOWN_KEY))
        {
            move2 += speed;
        }
        process_events();
        betting.display(move1, move2);
        refresh_screen(60);
    }

    write_line(to_string(move1) + " " + to_string(move2));
    return 0;
}

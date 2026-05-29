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
    bool drawn = false;
    bitmap face;
};


enum actions
{
    NO_ACTION = 0,
    CALL = 1,
    RAISE = 2,
    FOLD = 3
};


struct player
{
    string name;
    bitmap avatar;
    fixed_array<card *, 2> hand;
    bool user;
    bool fold = false;

    int hand_score;
    int last_action = NO_ACTION;
    int bet;
    int chips;

    int raise_amount;

    double image_x;
    double image_y;
};


struct p_round
{
    int pot = 0;
    int card_count = 0;
    int blind_i = 0;
    int call_amount = 0;
    int turn_i = 0;
};

class betting_display
{
private:

    #pragma region Constants

    const double img_height = bitmap_height(bitmap_named("p1"));
    bool flop;
    bool turn;
    bool river;
    const int CARD_DELAY = 500;
    const double fold_width = 133.33;
    const double fold_height = 60;
    const double fold_x = 266.66;
    const double fold_y = 800 - 100;
    // CHIPS DISPLAY
    const double chips_width = 1000 / 3;
    const double chips_height = 60;
    const double chips_x = fold_x + 1.25 * fold_width;
    const double chips_y = fold_y - chips_height - 5;
    // PLAYER CARDS
    double card_width = chips_width * 0.35;
    double card_height = (334 / 240) * card_width;
    double first_card_x = chips_x + chips_width / 10 - card_width / 2 + 8;
    double first_card_y = chips_y - card_height - 80;
    double card_scale_factor = 0.5;
    double card_spacing = 12;
    // CENTRE CARDS
    double center_card_x = 1200 / 2 - card_width * 2.5 - 2 * card_spacing - 65;
    double center_card_y = 240 - card_height / 2;

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

    // PLAYER CARDS
    rectangle card1 = {first_card_x, first_card_y, card_width, card_height};
    rectangle card2 = {first_card_x + card_width + card_spacing, first_card_y, card_width, card_height};

    // CENTRE CARDS

    // FLOP (FIRST 3 CARDS)
    rectangle centre_card1 = {center_card_x, center_card_y, card_width, card_height};
    rectangle centre_card2 = {center_card_x + card_width + card_spacing, center_card_y, card_width, card_height};
    rectangle centre_card3 = {center_card_x + 2 * (card_width + card_spacing), center_card_y, card_width, card_height};
    // TURN (4TH CARD)
    rectangle centre_card4 = {center_card_x + 3 * (card_width + card_spacing), center_card_y, card_width, card_height};
    // RIVER (5TH CARD)
    rectangle centre_card5 = {center_card_x + 4 * (card_width + card_spacing), center_card_y, card_width, card_height};
    #pragma endregion

    //PLAYER ACTIONS:

    rectangle action = {0,0, 100,40};
    

    void draw_button(const rectangle &button, const color &button_color, 
                    const string &label, const string &label2 = "")
    {
        fill_rectangle(button_color, button);

        int font_size = fold_height * 0.4;

        if (label2 == "")
        {
            double tw = text_width(label, "Roboto", font_size);
            double th = text_height(label, "Roboto", font_size);
            draw_text(label, COLOR_WHITE, "Roboto", font_size,
                    button.x + (button.width - tw) / 2,
                    button.y + (button.height - th) / 2);
        }
        else
        {
            double tw1 = text_width(label, "Roboto", font_size);
            double tw2 = text_width(label2, "Roboto", font_size);
            double th  = text_height(label, "Roboto", font_size);

            double total_height = th * 2;
            double start_y = button.y + (button.height - total_height) / 2;

            draw_text(label,  COLOR_WHITE, "Roboto", font_size,
                    button.x + (button.width - tw1) / 2, start_y);
            draw_text(label2, COLOR_WHITE, "Roboto", font_size,
                    button.x + (button.width - tw2) / 2, start_y + th);
        }
    }
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
            break;
        }

        return suit_result + rank_result + ".png";
    }


    void draw_text_bubble(const string &text, double x, double y)
    {
        int font_size = 16;
        double padding = 10;
        double tw = text_width(text, "Roboto", font_size);
        double th = text_height(text, "Roboto", font_size);

        double box_w = tw + padding * 2;
        double box_h = th + padding * 2;

        // Background
        fill_rectangle(COLOR_WHITE, x, y, box_w, box_h);
        draw_rectangle(COLOR_BLACK, x, y, box_w, box_h);

        // Tail (little triangle pointing down)
        fill_triangle(COLOR_WHITE, 
            x + box_w / 2 - 8, y + box_h,
            x + box_w / 2 + 8, y + box_h,
            x + box_w / 2,     y + box_h + 12);
        draw_line(COLOR_BLACK, x + box_w / 2 - 8, y + box_h, x + box_w / 2, y + box_h + 12);
        draw_line(COLOR_BLACK, x + box_w / 2 + 8, y + box_h, x + box_w / 2, y + box_h + 12);

        // Text
        draw_text(text, COLOR_BLACK, "Roboto", font_size, x + padding, y + padding);
    }

public:


    
    betting_display(const fixed_array<card *, 2> &player_cards, const fixed_array<card *, 5> &centre_cards)
    {
        write_line("LOADING FONT");
        load_font("Roboto", "Roboto.ttf");
        write_line("LOADED");

        flop = false;
        turn = false;
        river = false;
        write_line("CARD 1 lOADINg");
        load_bitmap("Player Card 1",card_to_display(player_cards[0]->suit, player_cards[0]->rank));
        load_bitmap("Player Card 2",card_to_display(player_cards[1]->suit, player_cards[1]->rank));
        write_line("LOADED");
        for (int i = 0; i < 5; i++)
        {
            load_bitmap("Centre Card " + to_string(i + 1), card_to_display(centre_cards[i]->suit,centre_cards[i]->rank));
        }
        write_line("LOADED");
        //PLAYER LOADING
        load_bitmap("p1", "player1.jpeg");
        load_bitmap("p2", "player2.jpeg");
        load_bitmap("p3", "player3.jpeg");
        write_line("LOADED");
        //BACK CARD LOAD
        load_bitmap("back", "back.png");


    }

    void draw_card_pair(const int &x, const int &y)
    {
        draw_bitmap("back", x + 430, y+ 45,option_scale_bmp(150.0/334.0,150.0/334.0));   // card 1
        draw_bitmap("back", x + 510, y + 45,option_scale_bmp(150.0/334.0,150.0/334.0));   // card 2
    }
    void draw_ai_cards(const int &i)
    {

        switch(i)
        {
            case 0:
                break;
            case 1: 
                draw_card_pair(490,200); // Right
                break;
            case 2:
                draw_card_pair(-108,-90); // Top
                break;
            case 3:
                draw_card_pair(-470,200); // Left
                break;
            default:
                break;
        }

    }



    void draw_action(const player &player)
    {
        string action;
        switch(player.last_action)
        {
            case NO_ACTION:
                break;
            case CALL:
                draw_text_bubble("CALL",player.image_x, player.image_y - 35.00);
                break;
            case RAISE:
                draw_text_bubble("RAISE: " + to_string(player.raise_amount),player.image_x, player.image_y - 35.00);
                break;
            case FOLD:
                draw_text_bubble("FOLD",player.image_x, player.image_y - 35.00);
                break;
            default:
                break;
        }
    }   
        
    void draw_chips(int chips, double x, double y)
    {
        int font_size = 16;
        double padding = 8;
        string text = "$ " + to_string(chips);
        double tw = text_width(text, "Roboto", font_size);
        double th = text_height(text, "Roboto", font_size);

        double box_w = tw + padding * 2;
        double box_h = th + padding * 2;

        double box_x = x + (150 - box_w) / 2;

        // dark blue outline
        fill_rectangle(rgb_color(20, 40, 80), box_x - 2, y - 2, box_w + 4, box_h + 4);
        // steel blue background
        fill_rectangle(rgb_color(70, 110, 180), box_x, y, box_w, box_h);
        // white text
        draw_text(text, COLOR_WHITE, "Roboto", font_size,
                box_x + padding, y + padding);
    }

    void draw_ai(const int &i, const player &player)
    {
        
        draw_action(player);
        
        if (player.last_action != FOLD) 
        {
            switch(i)
            {
                case 0:
                    break;
                case 1: 
                    draw_card_pair(490,200); // Right
                    break;
                case 2:
                    draw_card_pair(-108,-90); // Top
                    break;
                case 3:
                    draw_card_pair(-470,200); // Left
                    break;
                default:
                    break;
            }
        }

        draw_chips(player.chips, player.image_x, player.image_y + img_height + 155);
    }


    void draw_pot(int pot)
    {
        int font_size = 22;          // bigger font
        double padding = 12;         // more padding
        string text = "POT: $" + to_string(pot);
        double tw = text_width(text, "Roboto", font_size);
        double th = text_height(text, "Roboto", font_size);

        double box_w = tw + padding * 2;
        double box_h = th + padding * 2;

        double box_x = 1200 / 2.0 - box_w / 2;
        double box_y = 420;

        fill_rectangle(rgb_color(120, 90, 0), box_x - 2, box_y - 2, box_w + 4, box_h + 4);
        fill_rectangle(rgb_color(212, 175, 55), box_x, box_y, box_w, box_h);
        draw_text(text, rgb_color(60, 30, 0), "Roboto", font_size,
                box_x + padding, box_y + padding);
    }


    void draw_player_chips(int chips, double x, double y, double width, double height)
    {
        int font_size = 28;  // bigger font
        string text = "$ " + to_string(chips);
        double tw = text_width(text, "Roboto", font_size);
        double th = text_height(text, "Roboto", font_size);

        fill_rectangle(rgb_color(20, 40, 80), x - 2, y - 2, width + 4, height + 4);
        fill_rectangle(rgb_color(70, 110, 180), x, y, width, height);

        draw_text(text, COLOR_WHITE, "Roboto", font_size,
                x + (width - tw) / 2,
                y + (height - th) / 2);
    }
    void display(const p_round &round, const fixed_array<player, 4> &players, double move1 = 0, double move2 = 0,  const int &raise_amount = 50)
    {








        // PLAYER CARDS

        if (players[0].last_action != FOLD)
        {
            draw_bitmap(bitmap_named("Player Card 1"), card1.x, card1.y, option_scale_bmp(card_scale_factor, card_scale_factor));
            draw_bitmap(bitmap_named("Player Card 2"), card2.x, card2.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        }

        #pragma region Buttons
        if (round.turn_i == 0)
            {
            if (round.call_amount == 0)
            {
                draw_button(call_button, COLOR_ORANGE, "CHECK");
            }
            else
            {
            draw_button(call_button, COLOR_ORANGE, "Call" , to_string(round.call_amount)); 
            }
            draw_button(fold_button, COLOR_RED, "Fold");
            if (raise_amount == round.call_amount)
            {
                draw_button(minus_button, rgb_color(170, 130, 120), "-");  
            }
            else
            {
                draw_button(minus_button, COLOR_SALMON, "-"); 
            }
            
            if (round.call_amount + raise_amount >= players[0].chips - round.call_amount)
            {
                draw_button(plus_button, rgb_color(170, 130, 120), "+"); 
            }
            else
            {
                draw_button(plus_button, COLOR_SALMON, "+");
            }
            draw_button(raise_button, COLOR_PURPLE, "Raise" , to_string(round.call_amount + raise_amount));
        

            draw_button(all_in_button, COLOR_ORANGE, "ALL IN");
        }   

        
        draw_player_chips(players[0].chips, chips_x, chips_y, chips_width, chips_height);
        #pragma endregion




        for (int i = 0; i < round.card_count; i ++)
        {
            draw_bitmap(bitmap_named("Centre Card " + to_string(i+1)), centre_card1.x + i*(card_spacing +card_width), centre_card1.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        }






        




        draw_bitmap("p2", 590, 45);    // top centre (opponent)
        draw_bitmap("p3", 45, 175);    // left (opponent)
        draw_bitmap("p1", 1005, 175);  // right (opponent)
        


        
                // Draws AI CARDS
        for (int i = 1; i < 4; i++)
        {

            draw_ai(i, players[i]);

        }
        

        draw_pot(round.pot);


        
    }
};

class game
{
private:
    fixed_array<card, 52> deck;
    fixed_array<card *, 5> board_cards;
    fixed_array<player, 4> players;
    int current_cards = 0;
    const int BIG_BLIND = 20;
    const int SMALL_BLIND = BIG_BLIND * 0.5;
    int blind_i = 0;
    int pot = 0;


    // {
    bool flop;
    bool turn;
    bool river;
    const int CARD_DELAY = 500;
    const double fold_width = 133.33;
    const double fold_height = 60;
    const double fold_x = 266.66;
    const double fold_y = 800 - 100;
    // CHIPS DISPLAY
    const double chips_width = 1000 / 3;
    const double chips_height = 60;
    const double chips_x = fold_x + 1.25 * fold_width;
    const double chips_y = fold_y - chips_height - 5;
    // PLAYER CARDS
    double card_width = chips_width * 0.35;
    double card_height = (334 / 240) * card_width;
    double first_card_x = chips_x + chips_width / 10 - card_width / 2 + 8;
    double first_card_y = chips_y - card_height - 80;
    double card_scale_factor = 0.5;
    double card_spacing = 12;
    // CENTRE CARDS
    double center_card_x = 1200 / 2 - card_width * 2.5 - 2 * card_spacing - 65;
    double center_card_y = 240 - card_height / 2;

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
    // #endregion

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

        //INITIALISE

        for (int i = 0; i < 5; i++)
        {
            board_cards[i] = nullptr;
        }

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                players[i].hand[j] = nullptr;
            }
        }
        
        // FILL DECK
        for (int i = 0; i < 52; i++)
        {
            deck[i].suit = (suit_type)(i / 13);
            deck[i].rank = (rank_type)(i % 13 + 2);
        }


        //Player Images
        players[1].image_x = 1005;
        players[1].image_y = 175;
        players[2].image_x = 590;
        players[2].image_y = 45;
        players[3].image_x = 45;
        players[3].image_y = 175;

    }

    void reshuffle()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (players[i].hand[j] != nullptr)
                {
                    players[i].hand[j]->drawn = false;
                }
                players[i].hand[j] = nullptr;
            }
        }

        for (int i = 0; i < 5; i++)
        {   
            if (board_cards[i] != nullptr)
            {
                board_cards[i]->drawn = false;
            }

            board_cards[i] = nullptr;
        }
    }

    void deal_cards()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                players[i].hand[j] = deal();
            }
        }

        for (int i = 0; i < 5; i++)
        {
            board_cards[i] = deal();
        }
    }

    void insertion_sort(fixed_array<card *, 7> &cards)
    {
        for (int i = 1; i < 7; i++)
        {
            card *key = cards[i];

            int j = i - 1;

            while (j >= 0 && cards[j]->rank > key->rank)
            {
                cards[j + 1] = cards[j];
                j--;
            }
            cards[j + 1] = key;
        }
    }

    /// HAND EVALUATION

    int hand_hash(const int &hand, const int &first_high, const int &second_high)
    {
        return hand * 10000 + first_high * 100 + second_high;
    }

    int flush(const int (&suit)[4], const fixed_array<card *, 7> &cards)
    {
        for (int i = 0; i < 4; i++)
        {
            if (suit[i] == 5)
            {
                for (int j = 6; j > 3; j--)
                {
                    if (cards[j]->suit == i)
                    {
                        return hand_hash(FLUSH, cards[j]->rank, 0);
                    }
                }
            }
        }

        return -1;
    }

    int straight_flush(const fixed_array<card *, 7> &cards)
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
                return hand_hash(STRAIGHT_FLUSH, high, 0);
            }

            if (cards[i]->rank - 1 == cards[i - 1]->rank && cards[i]->suit == cards[i - 1]->suit)
            {
                if (tally == 0)
                {
                    high = cards[i]->rank;
                }
                tally++;
            }
        }

        return hand_hash(STRAIGHT_FLUSH, high, 0);
    }

    int straight(const int (&freq)[15])
    {
        int tally = 0;
        int high = 0;
        for (int i = 14; i > 1; i--)
        {
            if (tally == 5)
            {
                return hand_hash(STRAIGHT, high, 0);
            }

            if (tally == 0 && i == 4)
            {
                return -1;
            }

            if (freq[i] >= 3)
            {
                return -1;
            }

            if (freq[i] > 0 && freq[i - 1] > 0)
            {
                if (tally == 0)
                {
                    high = i;
                }
                tally++;
            }
        }

        return -1;
    }

    int multiple(const int (&freq)[15])
    {
        int twos = 0;
        int threes = 0;
        int fours = 0;
        int index_1 = 0;
        int index_2 = 0;
        for (int i = 2; i < 15; i++)
        {
            if (freq[i] == 2)
            {
                twos++;

                if (index_1 == 0)
                {
                    index_1 = i;
                }
                else
                {
                    // Switching orders, as if index 1 filled and we get another pair it MUST be larger
                    index_2 = index_1;
                    index_1 = i;
                }
            }
            if (freq[i] == 3)
            {
                threes++;

                if (index_1 == 0)
                {
                    index_1 = i;
                }
                else
                {
                    index_2 = index_1;
                    index_1 = i;
                }
            }
            if (freq[i] == 4)
            {
                fours++;

                index_1 = i;
            }
        }

        if (fours > 0)
        {
            return hand_hash(FOUR_OAK, index_1, 0);
        }
        else if (threes > 0 && twos > 0)
        {
            return hand_hash(FULL_HOUSE, index_1, index_2);
        }
        else if (threes > 0)
        {
            return hand_hash(THREE_OAK, index_1, 0);
        }
        else if (twos > 1)
        {
            return hand_hash(TWO_PAIR, index_1, index_2);
        }
        else if (twos > 0)
        {
            return hand_hash(PAIR, index_1, 0);
        }
        else
        {
            return -1;
        }
    }

    int high_card(const fixed_array<card *, 7> &cards)
    {
        return hand_hash(HIGH_CARD, cards[6]->rank, 0);
    }

    int hand_eval(player &player)
    {
        fixed_array<card *, 7> cards;

        // FILL CARDS
        for (int i = 0; i < 7; i++)
        {
            if (i < 5)
            {
                cards[i] = board_cards[i];
            }
            else
            {
                cards[i] = player.hand[i - 5];
            }
        }

        // SORT THEM
        insertion_sort(cards);

        int frequency[15] = {0};
        int suit[4] = {0};

        for (int i = 0; i < 7; i++)
        {
            frequency[cards[i]->rank]++;
            if (cards[i]->rank == 14)
            {
                frequency[1]++;
            }

            suit[cards[i]->suit]++;
        }

        // NOW TO RUN ALL CHECKS

        int hand_score;
        int multiple_score;

        hand_score = straight_flush(cards);
        if (hand_score != -1)
        {
            return hand_score;
        }

        multiple_score = multiple(frequency);

        // FOUR OF A KIND + FULL HOUSE
        if (multiple_score > 60000)
        {
            return multiple_score;
        }

        hand_score = flush(suit, cards);
        if (hand_score != -1)
        {
            return hand_score;
        }

        hand_score = straight(frequency);
        if (hand_score != -1)
        {
            return hand_score;
        }

        if (multiple_score != -1)
        {
            return multiple_score;
        }

        return high_card(cards);
    }




    //CLICK CHECK

    bool click_check(const rectangle &bound)
    {
        if (mouse_clicked(LEFT_BUTTON) && point_in_rectangle(mouse_position(),bound))
        {
            return true;
        }

        return false;
    }


    bool can_bet(int bet_amount, int index)
    {
        if (bet_amount > players[index].chips)
        {
            return false;
        }

        return true;
    }

    bool bet_complete()
    {
        return false;
    }

    void ai_turn(const p_round &round, const int &index )
    {
        
    }
    void player_turn(betting_display &display, p_round &round)
    {

    
        players[0].bet = 0;

        bool turn_complete = false;
        int min_raise = round.call_amount;
        int raise_amount = min_raise;
        int raise_increment = BIG_BLIND;

        while (!turn_complete && !quit_requested())
        {
            clear_screen(COLOR_GREEN);
            process_events();

            display.display(round, players, 0, 0, raise_amount);

            if(click_check(fold_button))
            {
                players[0].fold = true;
                players[0].bet = 0;
                turn_complete = true;
            }
            else if (click_check(all_in_button))
            {
                players[0].bet = players[0].chips;
                turn_complete = true;
            }
            else if (click_check(call_button) && round.call_amount <= players[0].chips)
            {
                players[0].bet = round.call_amount;
                turn_complete = true;
            }
            else if (click_check(minus_button) && raise_amount != min_raise)
            {
                if (raise_amount - raise_increment < min_raise)
                {
                    raise_amount = min_raise;
                }
                else
                {
                    raise_amount -= raise_increment;
                }
            }
            else if (click_check(plus_button))
            {
                raise_amount += raise_increment;
                if (raise_amount + round.call_amount > players[0].chips)
                {
                    raise_amount = players[0].chips - round.call_amount;
                }
            }
            else if (click_check(raise_button))
            {
                players[0].bet = raise_amount + round.call_amount;
                turn_complete = true;
            }
            
        }

        //PROCESS TURN

        players[0].chips -= players[0].bet;
        pot += players[0].bet;
    }


    //BUTTONS



    // BETTING

    bool betting_complete(int current_bet)
    {
        for (int i = 0; i < 4; i ++)
        {
            if (!players[i].fold && players[i].bet != current_bet)
            {
                return false;
            }
        }

        return true;
    }


    

    void game_round(int number_of_players)
    {
        for (int i = 0; i < number_of_players; i++)
        {
            players[i].bet = 0;
            players[i].fold = false;
        }

        players[blind_i].bet = SMALL_BLIND;
        players[blind_i].bet = BIG_BLIND;
    }



    fixed_array<card* , 5>& return_centre()
    {
        return board_cards;
    }

    player& return_player(int index)
    {
        return players[index];
    }

    fixed_array<player, 4>& return_players()
    {
        return players;
    }
};



int main()
{
    card card1;
    card card2;

    card1.suit = HEARTS;
    card1.rank = ACE;
    card2.suit = SPADES;
    card2.rank = KING;
    open_window("poker", 1200, 800);
    int move1 = 0;
    int move2 = 0;
    int speed = 3;

    game new_game = game();

    new_game.reshuffle();
    new_game.deal_cards();
    int call_amount = 20;
    new_game.return_player(0).chips = 400;
    betting_display betting(new_game.return_player(0).hand,new_game.return_centre());

    p_round new_round;

    new_round.blind_i = 0;
    new_round.call_amount = 50;
    new_round.card_count = 5;
    new_round.pot = 500;
    new_round.turn_i = 0;


    for (int i = 1; i < 4; i++)
    {
        new_game.return_players()[i].chips = rnd(1,5000);
    }
    new_game.return_players()[1].last_action = FOLD;
    
    new_game.return_players()[2].last_action = CALL;
    new_game.return_players()[3].raise_amount = 50;
    new_game.return_players()[3].last_action = RAISE;
    while (!quit_requested())
    {
        write_line("CHECK");
        clear_screen(COLOR_GREEN);
        write_line("CHECK");
        process_events();
        write_line("CHECK");
        if (key_down(LEFT_KEY))
        {
            move1 -= speed;
        }
        if (key_down(RIGHT_KEY))
        {
            move1 += speed;
        }
        if (key_down(UP_KEY))
        {
            move2 -= speed;
        }
        if (key_down(DOWN_KEY))
        {
            move2 += speed;
        }


        betting.display(new_round, new_game.return_players(), 0, 0, 50);
        write_line("CHECK");
        refresh_screen(60);
        //new_game.player_turn(call_amount,betting,5);
    }

    write_line(to_string(move1) + " " + to_string(move2));
    return 0;
}

#include "splashkit.h"
#include "splashkit-arrays.h"

const int BIG_BLIND = 20;
const int SMALL_BLIND = BIG_BLIND * 0.5;

enum round_state
{
    PRE_FLOP = 0,
    FLOP = 1,
    TURN = 2,
    RIVER = 3,
    SHOWDOWN = 4
};
enum suit_type
{
    CLUBS = 0,
    DIAMONDS = 1,
    HEARTS = 2,
    SPADES = 3
};

enum animation
{
    DEAL = 0,
    FLOP = 1,
    TURN = 2,
    RIVER = 3,
    
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

struct s_pot
{
    int amount = 0;
    fixed_array<bool, 4> eligible;
};

struct card
{
    suit_type suit;
    rank_type rank;
    bool drawn = false;
    bitmap face;
    bool out_check = false; // FOR USE IN CALCULATING OUTS
};

enum actions
{
    NO_ACTION = 0,
    CALL = 1,
    RAISE = 2,
    FOLD = 3,
    ALL_IN = 4,
    CHECK = 5
};

enum p_type
{
    TA = 0,
    TP = 1,
    LA = 2,
    LP = 3

    // TIGHT,LOOSE,PASSIVE,AGRESSIVE
};
struct player
{
    string name;
    bitmap avatar;
    fixed_array<card *, 2> hand;
    bool fold = false;
    int hand_score = 0;
    int last_action = NO_ACTION;
    int bet = 0;
    int chips;
    int raise_amount;
    double image_x;
    double image_y;
    p_type pers;
    int total_comp;
};

struct p_round
{
    fixed_array<s_pot, 3> pots;
    int card_count = 0;
    int blind_i = 0;
    int call_amount = 0;
    int pot = 0;
    int turn_i = 0;
    int sb_i;
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

    // PLAYER ACTIONS:

    rectangle action = {0, 0, 100, 40};

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
            double th = text_height(label, "Roboto", font_size);

            double total_height = th * 2;
            double start_y = button.y + (button.height - total_height) / 2;

            draw_text(label, COLOR_WHITE, "Roboto", font_size,
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
                      x + box_w / 2, y + box_h + 12);
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
        load_bitmap("Player Card 1", card_to_display(player_cards[0]->suit, player_cards[0]->rank));
        load_bitmap("Player Card 2", card_to_display(player_cards[1]->suit, player_cards[1]->rank));
        write_line("LOADED");
        for (int i = 0; i < 5; i++)
        {
            load_bitmap("Centre Card " + to_string(i + 1), card_to_display(centre_cards[i]->suit, centre_cards[i]->rank));
        }
        write_line("LOADED");
        // PLAYER LOADING
        load_bitmap("p1", "player1.jpeg");
        load_bitmap("p2", "player2.jpeg");
        load_bitmap("p3", "player3.jpeg");
        write_line("LOADED");
        // BACK CARD LOAD
        load_bitmap("back", "back.png");
    }

    void draw_card_pair(const int &x, const int &y)
    {
        draw_bitmap("back", x + 430, y + 45, option_scale_bmp(150.0 / 334.0, 150.0 / 334.0)); // card 1
        draw_bitmap("back", x + 510, y + 45, option_scale_bmp(150.0 / 334.0, 150.0 / 334.0)); // card 2
    }
    void draw_ai_cards(const int &i)
    {

        switch (i)
        {
        case 0:
            break;
        case 1:
            draw_card_pair(490, 200); // Right
            break;
        case 2:
            draw_card_pair(-108, -90); // Top
            break;
        case 3:
            draw_card_pair(-470, 200); // Left
            break;
        default:
            break;
        }
    }

    void draw_action(const player &player)
    {
        string action;
        switch (player.last_action)
        {
        case NO_ACTION:
            break;
        case CALL:
            draw_text_bubble("CALL", player.image_x, player.image_y - 35.00);
            break;
        case RAISE:
            draw_text_bubble("RAISE: " + to_string(player.raise_amount), player.image_x, player.image_y - 35.00);
            break;
        case FOLD:
            draw_text_bubble("FOLD", player.image_x, player.image_y - 35.00);
            break;
        case CHECK:
            draw_text_bubble("CHECK", player.image_x, player.image_y - 35.00);
        case ALL_IN:
            draw_text_bubble("ALL IN", player.image_x, player.image_y - 35.00);
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
            switch (i)
            {
            case 0:
                break;
            case 1:
                draw_card_pair(490, 200); // Right
                break;
            case 2:
                draw_card_pair(-108, -90); // Top
                break;
            case 3:
                draw_card_pair(-470, 200); // Left
                break;
            default:
                break;
            }
        }

        draw_chips(player.chips, player.image_x, player.image_y + img_height + 155);
    }

    void draw_pot(int pot)
    {
        int font_size = 22;  // bigger font
        double padding = 12; // more padding
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
        int font_size = 28; // bigger font
        string text = "$ " + to_string(chips);
        double tw = text_width(text, "Roboto", font_size);
        double th = text_height(text, "Roboto", font_size);

        fill_rectangle(rgb_color(20, 40, 80), x - 2, y - 2, width + 4, height + 4);
        fill_rectangle(rgb_color(70, 110, 180), x, y, width, height);

        draw_text(text, COLOR_WHITE, "Roboto", font_size,
                  x + (width - tw) / 2,
                  y + (height - th) / 2);
    }
    void display(const p_round &round, const fixed_array<player, 4> &players, double move1 = 0, double move2 = 0, const int &raise_amount, const int)
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
                draw_button(call_button, COLOR_ORANGE, "Call", to_string(round.call_amount));
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
            draw_button(raise_button, COLOR_PURPLE, "Raise", to_string(round.call_amount + raise_amount));

            draw_button(all_in_button, COLOR_ORANGE, "ALL IN");
        }

        draw_player_chips(players[0].chips, chips_x, chips_y, chips_width, chips_height);
#pragma endregion

        for (int i = 0; i < round.card_count; i++)
        {
            draw_bitmap(bitmap_named("Centre Card " + to_string(i + 1)), centre_card1.x + i * (card_spacing + card_width), centre_card1.y, option_scale_bmp(card_scale_factor, card_scale_factor));
        }



        draw_bitmap("p2", 590, 45);   // top centre (opponent)
        draw_bitmap("p3", 45, 175);   // left (opponent)
        draw_bitmap("p1", 1005, 175); // right (opponent)

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
    p_round round;
    betting_display display = betting_display(players[0].hand, board_cards);

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

    // Pre-flop equity lookup table
    // equity_table[rank1][rank2][suited]
    // rank1 >= rank2 always (2=TWO ... 14=ACE)
    // suited: 0 = offsuit, 1 = suited
    // values are whole number percentages from heads-up vs random hand table
    // pocket pairs use [r][r][0], the [r][r][1] slot is unused (set same value)

    // Index reference:
    // 2=TWO, 3=THREE, 4=FOUR, 5=FIVE, 6=SIX, 7=SEVEN
    // 8=EIGHT, 9=NINE, 10=TEN, 11=JACK, 12=QUEEN, 13=KING, 14=ACE

    // Array is sized [15][15][2] so ranks map directly to their int value
    // indices 0 and 1 are unused

    int max(const int &a, const int &b)
    {
        if (a >= b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }

    int min(const int &a, const int &b)
    {
        if (a <= b)
        {
            return a;
        }
        else
        {
            return b;
        }
    }
    float equity_table[15][15][2] =
        {
            // rows 0-1 unused
            {},
            {},

            // rank 2 (TWO) — only appears as lower card, row mostly unused
            // [2][2] = pocket 2s
            {
                {},
                {},       // [2][0], [2][1] unused
                {50, 50}, // [2][2] = 22
            },

            // rank 3 (THREE)
            {
                {},
                {},       // unused
                {36, 36}, // [3][2] = 32o, 32s
                {54, 54}, // [3][3] = 33
            },

            // rank 4 (FOUR)
            {
                {},
                {},
                {34, 37}, // [4][2] = 42o, 42s
                {37, 39}, // [4][3] = 43o, 43s
                {57, 57}, // [4][4] = 44
            },

            // rank 5 (FIVE)
            {
                {},
                {},
                {34, 38}, // [5][2] = 52o, 52s
                {37, 40}, // [5][3] = 53o, 53s
                {38, 41}, // [5][4] = 54o, 54s
                {60, 60}, // [5][5] = 55
            },

            // rank 6 (SIX)
            {
                {},
                {},
                {34, 38}, // [6][2] = 62o, 62s
                {36, 39}, // [6][3] = 63o, 63s
                {38, 41}, // [6][4] = 64o, 64s
                {40, 43}, // [6][5] = 65o, 65s
                {63, 63}, // [6][6] = 66
            },

            // rank 7 (SEVEN)
            {
                {},
                {},
                {35, 38}, // [7][2] = 72o, 72s
                {37, 40}, // [7][3] = 73o, 73s
                {38, 42}, // [7][4] = 74o, 74s
                {40, 44}, // [7][5] = 75o, 75s
                {42, 45}, // [7][6] = 76o, 76s
                {66, 66}, // [7][7] = 77
            },

            // rank 8 (EIGHT)
            {
                {},
                {},
                {37, 40}, // [8][2] = 82o, 82s
                {37, 41}, // [8][3] = 83o, 83s
                {39, 43}, // [8][4] = 84o, 84s
                {41, 44}, // [8][5] = 85o, 85s
                {43, 46}, // [8][6] = 86o, 86s
                {45, 48}, // [8][7] = 87o, 87s
                {69, 69}, // [8][8] = 88
            },

            // rank 9 (NINE)
            {
                {},
                {},
                {39, 42}, // [9][2] = 92o, 92s
                {40, 43}, // [9][3] = 93o, 93s
                {41, 44}, // [9][4] = 94o, 94s
                {43, 46}, // [9][5] = 95o, 95s
                {44, 47}, // [9][6] = 96o, 96s
                {46, 49}, // [9][7] = 97o, 97s
                {48, 51}, // [9][8] = 98o, 98s
                {72, 72}, // [9][9] = 99
            },

            // rank 10 (TEN)
            {
                {},
                {},
                {42, 45}, // [10][2] = T2o, T2s
                {42, 46}, // [10][3] = T3o, T3s
                {43, 46}, // [10][4] = T4o, T4s
                {44, 47}, // [10][5] = T5o, T5s
                {46, 49}, // [10][6] = T6o, T6s
                {48, 51}, // [10][7] = T7o, T7s
                {50, 52}, // [10][8] = T8o, T8s
                {51, 54}, // [10][9] = T9o, T9s
                {75, 75}, // [10][10] = TT
            },

            // rank 11 (JACK)
            {
                {},
                {},
                {44, 47}, // [11][2] = J2o, J2s
                {45, 48}, // [11][3] = J3o, J3s
                {46, 49}, // [11][4] = J4o, J4s
                {47, 50}, // [11][5] = J5o, J5s
                {50, 50}, // [11][6] = J6o, J6s
                {50, 52}, // [11][7] = J7o, J7s
                {51, 54}, // [11][8] = J8o, J8s
                {53, 56}, // [11][9] = J9o, J9s
                {55, 57}, // [11][10] = JTo, JTs
                {77, 77}, // [11][11] = JJ
            },

            // rank 12 (QUEEN)
            {
                {},
                {},
                {47, 50}, // [12][2] = Q2o, Q2s
                {48, 51}, // [12][3] = Q3o, Q3s
                {49, 52}, // [12][4] = Q4o, Q4s
                {50, 53}, // [12][5] = Q5o, Q5s
                {51, 54}, // [12][6] = Q6o, Q6s
                {52, 54}, // [12][7] = Q7o, Q7s
                {54, 56}, // [12][8] = Q8o, Q8s
                {55, 58}, // [12][9] = Q9o, Q9s
                {57, 59}, // [12][10] = QTo, QTs
                {58, 60}, // [12][11] = QJo, QJs
                {80, 80}, // [12][12] = QQ
            },

            // rank 13 (KING)
            {
                {},
                {},
                {50, 53}, // [13][2] = K2o, K2s
                {51, 54}, // [13][3] = K3o, K3s
                {52, 55}, // [13][4] = K4o, K4s
                {53, 56}, // [13][5] = K5o, K5s
                {54, 57}, // [13][6] = K6o, K6s
                {55, 57}, // [13][7] = K7o, K7s
                {56, 58}, // [13][8] = K8o, K8s
                {58, 60}, // [13][9] = K9o, K9s
                {60, 62}, // [13][10] = KTo, KTs
                {61, 63}, // [13][11] = KJo, KJs
                {61, 63}, // [13][12] = KQo, KQs
                {82, 82}, // [13][13] = KK
            },

            // rank 14 (ACE)
            {
                {},
                {},
                {55, 57}, // [14][2] = A2o, A2s
                {56, 58}, // [14][3] = A3o, A3s
                {57, 59}, // [14][4] = A4o, A4s
                {58, 60}, // [14][5] = A5o, A5s
                {58, 60}, // [14][6] = A6o, A6s
                {59, 61}, // [14][7] = A7o, A7s
                {60, 62}, // [14][8] = A8o, A8s
                {61, 63}, // [14][9] = A9o, A9s
                {63, 65}, // [14][10] = ATo, ATs
                {64, 65}, // [14][11] = AJo, AJs
                {64, 66}, // [14][12] = AQo, AQs
                {65, 67}, // [14][13] = AKo, AKs
                {85, 85}, // [14][14] = AA
            },
    };

    float initial_equity(player &player)
    {
        int suited = 0;
        rank_type r1 = player.hand[0]->rank;
        rank_type r2 = player.hand[1]->rank;

        if (r1 < r2)
        {
            rank_type temp = r1;
            r1 = r2;
            r2 = temp;
        }

        if (player.hand[0]->suit == player.hand[1]->suit)
        {
            suited = 1;
        }

        if (r1 == r2)
        {
            suited = 0;
        }

        float equity = equity_table[r1][r2][suited] / 100.0f;

        return equity;
    }

    int active_position()
    {
        int active_count = 0;
        for (int i = 0; i < 4; i++)
        {
            int seat = (round.blind_i + i) % 4;
            if (players[seat].chips > 0)
            {
                if (seat == round.turn_i)
                {
                    return active_count; // 0 = bb, 1 = utg, 2 = btn, 3 = sb
                }

                active_count++;
            }
        }

        return -1;
    }

    int active_players()
    {
        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            if (players[i].chips > 0)
            {
                count++;
            }
        }

        return count;
    }

    float p_equity_adjust(const player &player)
    {
        switch (player.pers)
        {
        case TA:
            return -0.05;
            break;
        case TP:
            return -0.1;
            break;
        case LA:
            return +0.1;
            break;
        case LP:
            return +0.05;
            break;
        default:
            return 0.0;
            break;
        }
    }

    int p_reraise(const player &player, const int &bet)
    {
        switch (player.pers)
        {
        case TA:
            return 3 * bet;
            break;
        case TP:
            return bet;
            break;
        case LA:
            return 4 * bet;
            break;
        case LP:
            return bet;
            break;
        default:
            return 0;
            break;
        }
    }

    float pot_odds(const int &bet)
    {
        if (bet == 0)
        {
            return 0.0;
        }
        return (float)bet / (round.pot + bet);
    }

    bool agressive(const player &player)
    {
        return player.pers == TA || player.pers == LA;
    }

public:
    game()
    {

        // INITIALISE

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

        // Player Images
        players[1].image_x = 1005;
        players[1].image_y = 175;
        players[2].image_x = 590;
        players[2].image_y = 45;
        players[3].image_x = 45;
        players[3].image_y = 175;

        // ROUND

        round.blind_i = 0;
        round.pot = 0;
        round.call_amount = 0;
        round.turn_i = 0;
        round.card_count = 0;
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
            if (players[i].chips > 0)
            {
                for (int j = 0; j < 2; j++)
                {
                    players[i].hand[j] = deal();
                }
            }
        }

        for (int i = 0; i < 5; i++)
        {
            board_cards[i] = deal();
        }
    }

    void insertion_sort(fixed_array<card *, 7> &cards, const int &total)
    {
        for (int i = 1; i < total; i++)
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

    int flush(const int (&suit)[4], const fixed_array<card *, 7> &cards, const int &total)
    {
        for (int i = 0; i < 4; i++)
        {
            if (suit[i] >= 5)
            {
                for (int j = total - 1; j > 3; j--)
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

    int straight_flush(const fixed_array<card *, 7> &cards, const int &total)
    {

        int tally = 0;
        int high = 0;
        for (int i = total - 1; i > 0; i--)
        {

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
            else
            {
                tally = 0;
                high = 0;
            }
        }

        return -1;
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

            if (freq[i] > 0 && freq[i - 1] > 0)
            {
                if (tally == 0)
                {
                    high = i;
                }
                tally++;
            }
            else
            {
                tally = 0;
                high = 0;
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

    int high_card(const fixed_array<card *, 7> &cards, const int &total)
    {
        return hand_hash(HIGH_CARD, cards[total - 1]->rank, 0);
    }

    int hand_eval(player &player, card *extra_card = nullptr)
    {

        fixed_array<card *, 7> cards;

        int total = round.card_count + 2;
        // FILL CARDS
        for (int i = 0; i < total; i++)
        {
            if (i < 2)
            {
                cards[i] = player.hand[i];
            }
            else
            {
                cards[i] = board_cards[i - 2];
            }
        }

        if (extra_card != nullptr && total < 7)
        {
            cards[total] = extra_card;
            total++;
        }

        // SORT THEM
        insertion_sort(cards, total);

        int frequency[15] = {0};
        int suit[4] = {0};

        for (int i = 0; i < total; i++)
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

        hand_score = straight_flush(cards, total);
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

        hand_score = flush(suit, cards, total);
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

        return high_card(cards, total);
    }

    bool is_top_pair(player &player)
    {
        int top_card = 0;
        for (int i = 0; i < round.card_count; i++)
        {
            if (board_cards[i]->rank > top_card)
            {
                top_card = board_cards[i]->rank;
            }
        }

        if (player.hand[0]->rank == top_card || player.hand[1]->rank == top_card)
        {
            return true;
        }
        else if (player.hand[0]->rank == player.hand[1]->rank && player.hand[0]->rank > top_card)
        {
            return true;
        }

        return false;
    }

    int count_outs(player &player)
    {
        int outs = 0;

        int current_hand_score = hand_eval(player);

        for (int i = 0; i < 2; i++)
        {
            player.hand[i]->out_check = true;
        }
        for (int i = 0; i < round.card_count; i++)
        {
            board_cards[i]->out_check = true;
        }

        for (int i = 0; i < 52; i++)
        {
            if (!deck[i].out_check)
            {
                if (current_hand_score / 10000 < hand_eval(player, &deck[i]) / 10000)
                {
                    outs++;
                }
            }
        }

        for (int i = 0; i < 2; i++)
        {
            player.hand[i]->out_check = false;
        }
        for (int i = 0; i < round.card_count; i++)
        {
            board_cards[i]->out_check = false;
        }

        return outs;
    }

    void player_bet(const int &index, const int &bet)
    {
        int to_add = bet - players[index].bet;
        to_add = min(to_add, players[index].chips);

        if (to_add < 0)
        {
            to_add = 0;
        }
        players[index].bet += to_add;
        players[index].chips -= to_add;

        if (players[index].chips == 0)
        {
            players[index].last_action = ALL_IN;
        }
    }

    void pre_flop()
    {

        float equity = pow(initial_equity(players[round.turn_i]), active_players() - 1) + p_equity_adjust(players[round.turn_i]);

        float cutoff;
        int position = active_position();

        switch (position)
        {
        case 0: // BIG BLIND
            cutoff = 0.42;
            break;
        case 1: // UTG
            cutoff = 0.5;
            break;
        case 2: // BTN
            cutoff = 0.42;
            break;
        case 3: // SMALL BLIND
            cutoff = 0.46;
            break;
        default:
            return;
        }

        int raise_amount = 0;

        switch (players[round.turn_i].pers)
        {
        case TA:
            raise_amount = 4 * BIG_BLIND;
            break;
        case TP:
            raise_amount = 3 * BIG_BLIND;
            break;
        case LA:
            raise_amount = 5 * BIG_BLIND;
            break;
        case LP:
            raise_amount = 3 * BIG_BLIND;
            break;
        default:
            raise_amount = 0;
        }

        if (position == 0)
        {
            raise_amount += 3 * BIG_BLIND;
        }
        else
        {
            raise_amount += (position - 1) * BIG_BLIND;
        }

        if (round.call_amount > BIG_BLIND)
        {
            int bet = p_reraise(players[round.turn_i], round.call_amount) + BIG_BLIND;

            if (equity >= pot_odds(bet))
            {
                round.call_amount = bet;
                players[round.turn_i].last_action = RAISE;
                players[round.turn_i].raise_amount = bet;
                player_bet(round.turn_i, bet);
            }
            else if (equity >= pot_odds(round.call_amount))
            {
                players[round.turn_i].last_action = CALL;
                bet = round.call_amount;
                player_bet(round.turn_i, bet);
            }
            else
            {
                players[round.turn_i].last_action = FOLD;
            }

            
        }
        else if (equity >= pot_odds(raise_amount))
        {
            round.call_amount = raise_amount;
            players[round.turn_i].last_action = RAISE;
            players[round.turn_i].raise_amount = raise_amount;

            player_bet(round.turn_i, raise_amount);
        }
        else if (equity >= cutoff)
        {
            players[round.turn_i].last_action = CALL;
            player_bet(round.turn_i, round.call_amount);
        }
        else
        {
            players[round.turn_i].last_action = FOLD;
        }
    }

    float spr()
    {
        int min_chips = players[0].chips;
        for (int i = 1; i < 4; i++)
        {
            if (!players[i].fold && players[i].chips < min_chips)
            {
                min_chips = players[i].chips;
            }
        }

        return (float)min_chips / round.pot;
    }

    int ft_spr_bet(player &player)
    {
        float personality_factor = 1.0f;

        if (agressive(player))
        {
            personality_factor = 1.25f;
        }
        else
        {
            personality_factor = 0.75f;
        }

        if (player.hand_score / 10000 >= STRAIGHT)
        {
            return min(player.chips, (int)(personality_factor * 0.75 * round.pot));
        }
        else if (player.hand_score / 10000 >= TWO_PAIR)
        {
            return min(player.chips, (int)(personality_factor * 0.6 * round.pot));
        }
        else if (is_top_pair(player))
        {
            return min(player.chips, (int)(personality_factor * 0.5 * round.pot));
        }

        return -1;
    }

    int outs()
    {

        if (round.card_count >= 5)
        {
            return -1;
        }
        float outs_fraction = count_outs(players[round.turn_i]) / 100.0f;
        float equity;
        if (round.card_count == 3) // FLOP
        {
            equity = outs_fraction * 4;
        }
        else if (round.card_count == 4) // TURN
        {
            equity = outs_fraction * 2;
        }

        equity += p_equity_adjust(players[round.turn_i]);

        if (equity >= pot_odds(round.call_amount))
        {
            if (outs_fraction >= 0.08 && agressive(players[round.turn_i]))
            {
                return min(players[round.turn_i].chips, (int)(0.66 * round.pot));
            }
            else
            {
                return round.call_amount;
            }
        }

        return -1;
    }

    int flop_and_turn_bet()
    {
        float spr = this->spr();

        players[round.turn_i].hand_score = hand_eval(players[round.turn_i]);
        int spr_bet = ft_spr_bet(players[round.turn_i]);

        float hand_type = players[round.turn_i].hand_score / 10000;
        if (spr < 3)
        {
            if (hand_type >= TWO_PAIR || (hand_type == PAIR && is_top_pair(players[round.turn_i])))
            {
                return players[round.turn_i].chips;
            }
        }
        else if (spr <= 8)
        {
            if (hand_type >= TWO_PAIR)
            {
                return spr_bet;
            }
        }
        else
        {
            if (hand_type >= STRAIGHT)
            {
                return spr_bet;
            }
        }

        int outs_bet = outs();

        if (outs_bet != -1)
        {
            return outs_bet;
        }

        if (!agressive(players[round.turn_i]))
        {
            return -1;
        }
        else if (players[round.turn_i].pers == TA && rnd(100) < 15)
        {

            return min(players[round.turn_i].chips, (int)(0.66 * round.pot));
        }
        else if (players[round.turn_i].pers == LA && rnd(100) < 30)
        {
            return min(players[round.turn_i].chips, (int)(0.66 * round.pot));
        }

        return -1;
    }

    void flop_and_turn()
    {
        int bet = flop_and_turn_bet();

        if (bet != -1)
        {
            bet = max(bet, round.call_amount);
            

            if (bet == round.call_amount)
            {
                players[round.turn_i].last_action = CALL;
            }
            else
            {
                round.call_amount = bet;
                players[round.turn_i].last_action = RAISE;
                players[round.turn_i].raise_amount = bet;
            }

            player_bet(round.turn_i, bet);
        }
        else
        {
            if (round.call_amount == 0)
            {
                players[round.turn_i].bet = 0;
                players[round.turn_i].last_action = CHECK;
            }
            else
            {
                players[round.turn_i].last_action = FOLD;
            }
        }
    }

    int river_bet()
    {

        players[round.turn_i].hand_score = hand_eval(players[round.turn_i]);

        int type = players[round.turn_i].hand_score / 10000;

        if (type >= STRAIGHT)
        {
            return min(round.pot, players[round.turn_i].chips);
        }
        else if (type >= TWO_PAIR)
        {
            return min((int)(0.75 * round.pot), players[round.turn_i].chips);
        }
        else if (type >= PAIR && pot_odds(round.call_amount) <= 0.33)
        {
            return round.call_amount;
        }
        else if (agressive(players[round.turn_i]))
        {
            if (rnd(100) < 20 && players[round.turn_i].pers == TA)
            {
                return min(players[round.turn_i].chips, (int)(0.85 * round.pot));
            }
            else if (rnd(100) < 35 && players[round.turn_i].pers == LA)
            {
                return min(players[round.turn_i].chips, (int)(0.95 * round.pot));
            }

            return -1;
        }

        return -1;
    }

    void river()
    {
        int bet = river_bet();

        if (bet != -1)
        {

            bet = max(bet, round.call_amount);

            

            if (bet == round.call_amount)
            {
                if (bet == 0)
                {
                    players[round.turn_i].last_action = CHECK;
                }
                else
                {
                    players[round.turn_i].last_action = CALL;
                }
            }
            else
            {
                round.call_amount = bet;
                players[round.turn_i].last_action = RAISE;
                players[round.turn_i].raise_amount = bet;
            }

            player_bet(round.turn_i, bet);

        }
        else
        {
            if (round.call_amount == 0)
            {
                players[round.turn_i].bet = 0;
                players[round.turn_i].last_action = CHECK;
            }
            else
            {
                players[round.turn_i].last_action = FOLD;
            }
        }
    }
    // CLICK CHECK

    bool click_check(const rectangle &bound)
    {
        if (mouse_clicked(LEFT_BUTTON) && point_in_rectangle(mouse_position(), bound))
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

    void player_turn()
    {


        bool turn_complete = false;
        int min_raise = round.call_amount;
        int raise_amount = min_raise;
        int raise_increment = BIG_BLIND;
        int bet;
        while (!turn_complete && !quit_requested())
        {
            clear_screen(COLOR_GREEN);
            process_events();

            display.display(round, players, 0, 0, raise_amount);

            if (click_check(fold_button))
            {
                players[0].fold = true;
                turn_complete = true;
            }
            else if (click_check(all_in_button))
            {
                player_bet(0, players[0].chips);
                turn_complete = true;
            }
            else if (click_check(call_button) && round.call_amount <= players[0].chips)
            {
                player_bet(0, round.call_amount);
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
                player_bet(0, raise_amount + round.call_amount);
                turn_complete = true;
            }
        }
    }

    // BUTTONS

    // PRE-FLOP EQUITY

    // BETTING

    void new_round()
    {
        round.blind_i = (round.blind_i + 1) % 4;
        for (int i = 0; i < 4; i++)
        {
            if(players[i].chips > 0)
            {
                players[i].fold = false;
            }
            else
            {
                players[i].fold = true;
            }

            players[i].hand_score = 0;
            players[i].last_action = NO_ACTION;
            players[i].bet = 0;
            players[i].raise_amount = 0;
            players[i].total_comp = 0;
        }

        round.card_count = 0;

        for (int i = 0; i < 3; i++)
        {
            round.pots[i].amount = 0;
            for (int j = 0; j < 4; j++)
            {
                round.pots[i].eligible[j] = false;
            }
        }
    }

    void r_state_switch()
    {
        if(round.turn_i == 0)
        {
            player_turn();
        }
        else
        {
            switch (round.card_count)
            {
            case 0:
                pre_flop();
                break;
            case 3:
                flop_and_turn();
                break;
            case 4:
                flop_and_turn();
                break;
            case 5:
                river();
                break;
            default:
                break;
            } 
        }
    }

    void blind_bet(const int &blind, const int &index)
    {
        int bet = min(players[index].chips, blind);

        player_bet(index, bet);
    }

    bool active(const int &i)
    {
        if (players[i].last_action != FOLD && players[i].last_action != ALL_IN && players[i].chips > 0) 
        {
            return true;
        }
        
        return false;
    }

    int active_betters()
    {
        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            if (active(i)) 
            {
                count ++;
            }
        }

        return count;
    }

    void r_cycle()
    {

        if (round.card_count == 0)
        {
            round.call_amount = BIG_BLIND;
            blind_bet(BIG_BLIND, round.blind_i);
            blind_bet(SMALL_BLIND, round.sb_i);

            round.turn_i = (round.blind_i + 1) % 4;

            while (players[round.turn_i].chips <= 0)
            {
                round.turn_i = (round.turn_i + 1) % 4;
            }
        }
        else
        {
            round.turn_i = round.sb_i;
            round.call_amount = 0;
        }


        int to_act = active_betters();


        while (to_act != 0 && active_betters() > 1)
        {
            

            if(active(round.turn_i))
            {
                int call = round.call_amount;
                r_state_switch();


                if( call < round.call_amount)
                {
                to_act = active_betters() - 1;
                }
                else
                {
                to_act --;
                }
            }

            round.turn_i = (round.turn_i + 1) % 4;

        }



        for (int i = 0; i < 4; i++)
        {
            round.pot += players[i].bet;
            players[i].total_comp += players[i].bet;
            players[i].bet = 0;
        }
        
    }


    int find_min_comp()
    {
        int min_comp = -1;

        for (int i = 0; i < 4; i++)
        {
            if (players[i].total_comp > 0)
            {
                if (min_comp == -1 || players[i].total_comp < min_comp)
                {
                    min_comp = players[i].total_comp;
                }
            }
        }

        return min_comp;
    }

    fixed_array<bool, 4> showdown(const int &pot_index)
    {
        fixed_array<bool, 4> winners;
        int best = -1;
        for (int i = 0; i < 4; i++)
        {
            winners[i] = false;

            if (round.pots[pot_index].eligible[i])
            {
                players[i].hand_score = hand_eval(players[i]);

                if (players[i].hand_score > best)
                {
                    best = players[i].hand_score;
                }
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (round.pots[pot_index].eligible[i] && players[i].hand_score == best)
            {
                winners[i] = true;
            }
        }

        return winners;
    }

    void pot_split()
    {
        int pot_index = 0;
        int min_comp = 0;
        fixed_array<bool, 4> winners;
        while (pot_index < 3)
        {
            min_comp = find_min_comp();

            if (min_comp == -1)
            {
                break;
            }
            for (int i = 0; i < 4; i++)
            {
                if (players[i].total_comp >= min_comp)
                {
                    round.pots[pot_index].amount += min_comp;
                    players[i].total_comp -= min_comp;

                    if (players[i].last_action != FOLD)
                    {
                        round.pots[pot_index].eligible[i] = true;
                    }
                }
            }

            winners = showdown(pot_index);
            int count = 0;
            for (int i = 0; i < 4; i++)
            {
                if (winners[i])
                {
                    count++;
                }
            }

            if (count == 0)
            {
                count = 1;
            }

            int split = round.pots[pot_index].amount / count;
            for (int i = 0; i < 4; i++)
            {

                if (winners[i])
                {
                    players[i].chips += split;
                }
            }

            pot_index++;
        }
    }
    void round_runner()
    {
        new_round();
        reshuffle();
        deal_cards();

        if (active_players() < 4)
        {
            while (players[round.blind_i].chips <= 0)
            {
                round.blind_i = (round.blind_i + 1) % 4;
            }


        }

        round.sb_i = (round.blind_i - 1 + 4) % 4;

        while (players[round.sb_i].chips <= 0)
        {
            round.sb_i = (round.sb_i - 1 + 4) % 4;
        }

        if (active_betters() > 1)
        {
            r_cycle();
        }

        round.card_count = 3;
        while (active_betters() > 1 && round.card_count < 5) 
        {
            r_cycle();
            round.card_count ++;
        }

        if (active_betters() > 1)
        {
            r_cycle();
        }



        pot_split();
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
    betting_display betting(new_game.return_player(0).hand, new_game.return_centre());

    p_round new_round;

    new_round.blind_i = 0;
    new_round.call_amount = 50;
    new_round.card_count = 5;
    new_round.pot = 500;
    new_round.turn_i = 0;

    for (int i = 1; i < 4; i++)
    {
        new_game.return_players()[i].chips = rnd(1, 5000);
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
        // new_game.player_turn(call_amount,betting,5);
    }

    write_line(to_string(move1) + " " + to_string(move2));
    return 0;
}
add all in and check to visuals for ai and player.

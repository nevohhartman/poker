class ai
{
private:
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

    int active_position(fixed_array<player, 4> &players, p_round &round, const int &index)
    {
        int active_count = 0;
        for (int i = 0; i < 4; i++)
        {
            int seat = (round.blind_i + i) % 4;
            if (players[seat].chips > 0)
            {
                if (seat == index)
                {
                    return active_count; // 0 = bb, 1 = utg, 2 = btn, 3 = sb
                }

                active_count++;
            }
        }

        return -1;
    }

    int active_players(const fixed_array<player, 4> &players)
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

    float pot_odds(const p_round &round, const int &bet)
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
    void pre_flop(fixed_array<player, 4> &players, p_round &round, const int &index)
    {

        float equity = pow(initial_equity(players[index]), active_players(players) - 1) + p_equity_adjust(players[index]);

        float cutoff;
        int position = active_position(players, round, index);

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

        switch (players[index].pers)
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
            int bet = p_reraise(players[index], round.call_amount) + BIG_BLIND;

            if (equity >= pot_odds(round, bet))
            {
                round.call_amount = bet;
                players[index].last_action = RAISE;
                players[index].raise_amount = bet;
            }
            else
            {
                players[index].last_action = FOLD;
            }
        }
        else if (equity >= pot_odds(round, raise_amount))
        {
            round.call_amount = raise_amount;
            players[index].last_action = RAISE;
            players[index].raise_amount = raise_amount;
        }
        else if (equity >= cutoff)
        {
            players[index].last_action = CALL;
            players[index].bet = round.call_amount;
        }
        else
        {
            players[index].last_action = FOLD;
        }
    }
};
/*
 * ============================================================
 *   HANGMAN GAME IN C
 *   - 50 built-in words across 5 categories
 *   - 6 wrong guesses allowed
 *   - ASCII art gallows updated live
 *   - Tracks guessed letters, hints, score
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WRONG    6
#define MAX_WORDS    50
#define MAX_LEN      30

/* ── Word bank with hints ── */
typedef struct {
    char word[MAX_LEN];
    char hint[80];
    char category[20];
} WordEntry;

WordEntry wordBank[MAX_WORDS] = {
    /* Animals */
    {"elephant",   "Largest land animal",              "Animal"},
    {"giraffe",    "Tallest animal on Earth",           "Animal"},
    {"dolphin",    "Intelligent marine mammal",         "Animal"},
    {"cheetah",    "Fastest land animal",               "Animal"},
    {"penguin",    "A flightless bird from Antarctica", "Animal"},
    {"kangaroo",   "Has a pouch for its young",         "Animal"},
    {"crocodile",  "Ancient reptile with strong jaws",  "Animal"},
    {"butterfly",  "Insect with colorful wings",        "Animal"},
    {"octopus",    "Eight-armed sea creature",          "Animal"},
    {"rhinoceros", "Has a horn on its nose",            "Animal"},

    /* Countries */
    {"brazil",     "Largest country in South America",  "Country"},
    {"japan",      "Land of the Rising Sun",            "Country"},
    {"australia",  "Country and a continent",           "Country"},
    {"canada",     "Known for maple syrup",             "Country"},
    {"egypt",      "Home of the pyramids",              "Country"},
    {"france",     "Home of the Eiffel Tower",          "Country"},
    {"india",      "Land of spices and diversity",      "Country"},
    {"mexico",     "Known for tacos and mariachi",      "Country"},
    {"norway",     "Land of the Northern Lights",       "Country"},
    {"turkey",     "Spans two continents",              "Country"},

    /* Sports */
    {"cricket",    "Bat-and-ball game, popular in Asia","Sport"},
    {"basketball", "Invented by James Naismith",        "Sport"},
    {"volleyball", "Net sport with six players a side", "Sport"},
    {"swimming",   "Olympic aquatic discipline",        "Sport"},
    {"badminton",  "Played with a shuttlecock",         "Sport"},
    {"gymnastics", "Combines strength and flexibility", "Sport"},
    {"wrestling",  "Ancient combat sport",              "Sport"},
    {"archery",    "Shooting arrows at a target",       "Sport"},
    {"cycling",    "Tour de France is its famous race", "Sport"},
    {"baseball",   "America's favorite as time",       "Sport"},

    /* Technology */
    {"keyboard",   "Input device for a computer",      "Tech"},
    {"internet",   "Global network of networks",       "Tech"},
    {"algorithm",  "Step-by-step problem-solving rule","Tech"},
    {"database",   "Organized collection of data",     "Tech"},
    {"compiler",   "Translates code to machine language","Tech"},
    {"bandwidth",  "Data transfer capacity",           "Tech"},
    {"python",     "Popular high-level language",      "Tech"},
    {"ethernet",   "Common wired network standard",    "Tech"},
    {"firewall",   "Network security barrier",         "Tech"},
    {"processor",  "Brain of the computer",            "Tech"},

    /* Food */
    {"spaghetti",  "Long pasta from Italy",            "Food"},
    {"chocolate",  "Sweet made from cacao beans",      "Food"},
    {"avocado",    "Green fruit used in guacamole",    "Food"},
    {"pancake",    "Flat cake cooked on a griddle",    "Food"},
    {"sushi",      "Japanese rice and seafood dish",   "Food"},
    {"croissant",  "Flaky French pastry",              "Food"},
    {"blueberry",  "Small blue antioxidant-rich fruit","Food"},
    {"cinnamon",   "Warm spice from tree bark",        "Food"},
    {"dumpling",   "Dough filled with meat or veggies","Food"},
    {"tiramisu",   "Italian coffee-flavoured dessert", "Food"},
};

/* ── ASCII Hangman stages (0 = safe, 6 = dead) ── */
void drawHangman(int wrong) {
    const char *stages[7][7] = {
        {
            "  +---+  ",
            "  |   |  ",
            "         ",
            "         ",
            "         ",
            "         ",
            "=========",
        },
        {
            "  +---+  ",
            "  |   |  ",
            "  O      ",
            "         ",
            "         ",
            "         ",
            "=========",
        },
        {
            "  +---+  ",
            "  |   |  ",
            "  O      ",
            "  |      ",
            "         ",
            "         ",
            "=========",
        },
        {
            "  +---+  ",
            "  |   |  ",
            "  O      ",
            " /|      ",
            "         ",
            "         ",
            "=========",
        },
        {
            "  +---+  ",
            "  |   |  ",
            "  O      ",
            " /|\\    ",
            "         ",
            "         ",
            "=========",
        },
        {
            "  +---+  ",
            "  |   |  ",
            "  O      ",
            " /|\\    ",
            " /       ",
            "         ",
            "=========",
        },
        {
            "  +---+  ",
            "  |   |  ",
            "  O      ",
            " /|\\    ",
            " / \\    ",
            "         ",
            "=========",
        },
    };
    printf("\n");
    for (int i = 0; i < 7; i++)
        printf("  %s\n", stages[wrong][i]);
    printf("\n");
}

/* ── Display word with blanks ── */
int displayWord(const char *word, const char *guessed, int len) {
    int revealed = 0;
    printf("  Word: ");
    for (int i = 0; i < len; i++) {
        if (strchr(guessed, word[i])) {
            printf("%c ", toupper(word[i]));
            revealed++;
        } else {
            printf("_ ");
        }
    }
    printf("\n");
    return (revealed == len);
}

/* ── Display guessed letters ── */
void displayGuessed(const char *guessed) {
    printf("  Guessed: ");
    for (int i = 0; guessed[i] != '\0'; i++)
        printf("%c ", toupper(guessed[i]));
    printf("\n");
}

/* ── Divider ── */
void printLine(char c, int n) {
    for (int i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

/* ── Single game ── */
int playGame(int *totalScore) {
    /* Pick random word */
    int idx = rand() % MAX_WORDS;
    const char *word     = wordBank[idx].word;
    const char *hint     = wordBank[idx].hint;
    const char *category = wordBank[idx].category;
    int wordLen = strlen(word);

    char guessed[27] = "";   /* letters guessed so far */
    int  wrong       = 0;
    int  hintUsed    = 0;

    printLine('=', 58);
    printf("   *** HANGMAN ***   Category: %-12s\n", category);
    printLine('=', 58);

    while (wrong < MAX_WRONG) {
        drawHangman(wrong);
        printf("  Lives left : %d / %d\n", MAX_WRONG - wrong, MAX_WRONG);
        printf("  Letters    : %d\n", wordLen);
        displayGuessed(guessed);
        printLine('-', 58);

        int won = displayWord(word, guessed, wordLen);
        if (won) {
            printLine('=', 58);
            printf("  *** YOU WIN! *** The word was: %s\n", word);
            int points = (MAX_WRONG - wrong) * 10 + (hintUsed ? 0 : 20);
            *totalScore += points;
            printf("  Points earned this round : %d\n", points);
            printf("  Total score              : %d\n", *totalScore);
            printLine('=', 58);
            return 1;
        }

        printf("\n  [H] Use Hint   [A-Z] Guess a letter\n");
        printf("  Your input: ");
        char input[10];
        scanf(" %s", input);
        char ch = tolower(input[0]);

        /* Hint request */
        if (ch == 'h') {
            if (!hintUsed) {
                printf("\n  HINT: %s\n", hint);
                hintUsed = 1;
            } else {
                printf("\n  [!] Hint already used!\n");
            }
            continue;
        }

        /* Validate letter */
        if (!isalpha(ch)) {
            printf("\n  [!] Enter a valid letter (A-Z).\n");
            continue;
        }

        /* Already guessed? */
        if (strchr(guessed, ch)) {
            printf("\n  [!] You already guessed '%c'.\n", toupper(ch));
            continue;
        }

        /* Record guess */
        int gLen = strlen(guessed);
        guessed[gLen]     = ch;
        guessed[gLen + 1] = '\0';

        /* Check if in word */
        if (strchr(word, ch)) {
            printf("\n  ✓ Correct! '%c' is in the word.\n", toupper(ch));
        } else {
            wrong++;
            printf("\n  ✗ Wrong! '%c' is NOT in the word. (%d wrong)\n",
                   toupper(ch), wrong);
        }
    }

    /* Game over */
    drawHangman(MAX_WRONG);
    printLine('=', 58);
    printf("  *** GAME OVER ***  The word was: ");
    for (int i = 0; i < wordLen; i++) printf("%c", toupper(word[i]));
    printf("\n");
    printLine('=', 58);
    return 0;
}

/* ── Main ── */
int main(void) {
    srand((unsigned)time(NULL));

    char playerName[50];
    int  totalScore = 0;
    int  wins = 0, losses = 0;
    char again;

    printLine('=', 58);
    printf("        WELCOME TO HANGMAN IN C!\n");
    printLine('=', 58);
    printf("  Rules:\n");
    printf("  - Guess the hidden word one letter at a time.\n");
    printf("  - You have 6 wrong guesses before game over.\n");
    printf("  - Type H to get a hint (costs 20 bonus points).\n");
    printf("  - Score = lives remaining x10 + (no-hint bonus 20).\n");
    printLine('=', 58);
    printf("\n  Enter your name: ");
    scanf(" %[^\n]", playerName);

    do {
        int won = playGame(&totalScore);
        if (won) wins++; else losses++;

        printf("\n  Play again? (Y/N): ");
        scanf(" %c", &again);
        printf("\n");

    } while (tolower(again) == 'y');

    /* Final summary */
    printLine('=', 58);
    printf("   FINAL SUMMARY – %s\n", playerName);
    printLine('=', 58);
    printf("  Games Won    : %d\n", wins);
    printf("  Games Lost   : %d\n", losses);
    printf("  Total Score  : %d\n", totalScore);
    printf("  Win Rate     : %.0f%%\n",
           (wins + losses) > 0
               ? (float)wins / (wins + losses) * 100
               : 0.0f);
    printLine('=', 58);
    printf("  Thanks for playing, %s! See you next time.\n", playerName);
    printLine('=', 58);
    printf("\n");

    return 0;
}
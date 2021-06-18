/*

Authors: Son Mac and Thu Tra
Date created: April 28th 2020
Description: this program is a single-player card game called 31. The dealer will be played automatically, and the player will be played by user.
Rules are specify throughout the code.

*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


typedef struct card_s {
	char suit[9]; // character denoting suit: 's' for spade, 'h' for heart, 'd' for diamond, 'c' for club
	int suitVal; // an integer from 1 to 4: 1 = spade, 2 = heart, 3 = diamond, 4 = club
	int value; // from 1 (ace) to 13 (J = 11, Q = 12, K = 13)
	int order; // order of each card, starting from ace of spade as 1
	int character; // 0 means visible, 1 means invisible
	int aceCharacter; //0 means 1, 1 means 11
	struct card_s* next, * previous;
}card;

void swapCardorder(card* h, int order1, int order2); //swap two cards in a deck using order
int handTotalwithoutAce(card* h); //calculate the total value of a hand without the value of Ace
int countAce(card* h); //count the number of Ace(s) in a hand
void revealCard(card* h); //reveal all the hidden cards
void deleteList(card* h); //delete a full list
int countCard(card* h); //count the number of cards in the list
int handTotal(card* h); //calculate the total value of a hand
void deleteCard(card** hr, card** hl, card* temp); //delete a desired card from a hand
void addCard(card** h, card** t, card* temp); //add a card to a hand
void swapCard(card* h, int val1, int suit1, int val2, int suit2); //swap two cards in a deck using value and suit
void printList(card* h); //print all the cards in a deck
card* drawCard(card* h); //pick a random card from a deck


int main() {
	card* headl = NULL, * headr = NULL, * temp = NULL; // card from the original deck
	card* headd = NULL, * taild = NULL, * tempd = NULL; // card from dealer's hand
	card* headp = NULL, * tailp = NULL, * tempp = NULL; // card from player's hand
	int suit, value, swaptime, suit1, suit2, val1, val2, order1, order2;
	char new_game = 'y';
	int numGame = 0;
	double numRound = 0, playerWin = 0, dealerWin = 0, playerWin_14 = 0, numHand_27 = 0, numHand = 0;

	while ((new_game == 'y') || (new_game == 'Y')) {

		numGame++; //count the number of games played

		char create_deck = 'a';
		while ((create_deck != 'I') && (create_deck != 'i') && (create_deck != 'C') && (create_deck != 'c')) { // user can choose to create and shuffle a new deck or import a test file
			printf("Hi player, welcome to the game!\n");
			printf("The program will either create a new deck or let you import a predefined deck. Which one do you want ? Import (I) or Create (C) ");
			scanf(" %c", &create_deck);
		}
		if ((create_deck == 'C') || (create_deck == 'c')) { // create the original decks (two decks) in order
			for (value = 1; value <= 26; ++value) {
				for (suit = 1; suit <= 4; ++suit) {
					temp = (card*)malloc(sizeof(card));
					temp->value = value;
					temp->suitVal = suit;
					temp->order = 4 * value + suit - 4;
					temp->character = 0;
					if (headl == NULL) {
						temp->previous = NULL;
						headl = temp;
						headr = temp;
					}
					else {
						temp->previous = headr;
						headr = temp;
						temp->previous->next = temp;
					}
					temp->next = NULL;
				}
			}

			srand(time(NULL)); // shuffle the deck by swapping pairs of cards 100 times
			for (swaptime = 0; swaptime < 100; swaptime++) {
				suit1 = (rand() % 4) + 1;
				val1 = (rand() % 26) + 1;
				suit2 = (rand() % 4) + 1;
				val2 = (rand() % 26) + 1;
				swapCard(headl, val1, suit1, val2, suit2);
			}
		}
		if ((create_deck == 'I') || (create_deck == 'i')) {
			char filename[50];
			int counter = 0;
			printf("Enter the input file name: ");
			scanf("%s", filename);
			FILE* inp = fopen(filename, "r");
			if (inp == NULL) {
				printf("This file does not exist.\n");
				exit(0);
			}
			else {
				while (!feof(inp)) {
					temp = (card*)malloc(sizeof(card));
					fscanf(inp, "%d%s", &temp->value, temp->suit);
					if (strcmp(temp->suit, "s") == 0) {
						temp->suitVal = 1;
					}
					else if (strcmp(temp->suit, "h") == 0) {
						temp->suitVal = 2;
					}
					else if (strcmp(temp->suit, "d") == 0) {
						temp->suitVal = 3;
					}
					else {
						temp->suitVal = 4;
					}
					counter++;
					temp->order = counter;
					temp->character = 0;
					if (headl == NULL) {
						temp->previous = NULL;
						headl = temp;
						headr = temp;
					}
					else {
						temp->previous = headr;
						headr = temp;
						temp->previous->next = temp;
					}
					temp->next = NULL;
				}
			}
			if (counter != 104) { //check if the file contains a valid number of cards
				printf("The file does not contain a proper number of cards. Please run the program again.\n");
				exit(0);
			}

			srand(time(NULL)); //shuffle the deck by swapping pairs of cards 100 times
			for (swaptime = 0; swaptime < 100; swaptime++) {
				order1 = (rand() % 104) + 1;
				order2 = (rand() % 104) + 1;
				while (order1 == order2) {
					order1 = (rand() % 104) + 1;
					order2 = (rand() % 104) + 1;
				}
				swapCardorder(headl, order1, order2);
			}
		}

		char s_or_c = 'c'; //continue or stop 
		int round = 1; //initialize the value of round
		int bet, moneyLeft = 1000, cardCounter = 0;

		printf("\nLET'S PLAY 31!\n\n");
		printf("Note 1: You are starting the game with $%d in your balance. You are not allowed to bet lower than $20 or higher than $200 for each round.\n", moneyLeft);
		printf("Note 2: If you drew an Ace, or some Aces, you are allowed to choose the value of it, or all of them, after each new draw.\n");
		printf("Note 3: The %c indicates a hidden card.\n", 254);
		printf("Note 4: Please enable Raster font in the console box to see the graphic of the game.\n");
		printf("Note 5: Have fun playing!\n");


		while (s_or_c == 'c' || s_or_c == 'C') { //THE GAME STARTS HERE

			cardCounter = countCard(headl); //count the remaining cards in the original deck

			if (cardCounter < 30) { //recreate the initial deck and shuffle it again if there are less than 30 cards left
				printf("There are less than 30 cards left. The deck will be re-created then shuffled.\n");
				deleteList(headl);
				headl = NULL;
				headr = NULL;
				for (value = 1; value <= 26; ++value) { // creating the original decks (two decks) in order
					for (suit = 1; suit <= 4; ++suit) {
						temp = (card*)malloc(sizeof(card));
						temp->value = value;
						temp->suitVal = suit;
						temp->order = 4 * value + suit - 4;
						temp->character = 0;
						if (headl == NULL) {
							temp->previous = NULL;
							headl = temp;
							headr = temp;
						}
						else {
							temp->previous = headr;
							headr = temp;
							temp->previous->next = temp;
						}
						temp->next = NULL;
					}
				}
				srand(time(NULL)); // shuffle the deck by swapping pairs of cards
				for (swaptime = 0; swaptime < 50; swaptime++) {
					suit1 = (rand() % 4) + 1;
					val1 = (rand() % 26) + 1;
					suit2 = (rand() % 4) + 1;
					val2 = (rand() % 26) + 1;
					swapCard(headl, val1, suit1, val2, suit2);
				}
			}

			int aceAdd = 0, totalp = 0, aceLoop; //aceAdd variable is used to deal with unsure value of Ace
			char o_or_e;

			printf("\n==================ROUND %d==================\n\n", round);
			srand(time(NULL)); // draw a card for dealer
			tempd = drawCard(headl); //pick a random card from the original deck
			addCard(&headd, &taild, tempd); //add the card to the dealer's hand
			tempd->character = 1; //make the card invisible
			printf("Dealer: ");
			printList(headd); //print the dealer's hand
			numHand++; // count the total number of hands
			printf("\n");
			deleteCard(&headl, &headr, tempd); // delelte the drawn card from the original deck

			//draw a card for the player 
			tempp = drawCard(headl); //pick a random card from the original deck
			addCard(&headp, &tailp, tempp); //add the card to the player's hand
			printf("Player: ");
			printList(headp); //print the player's hand
			numHand++; // count the total number of hands
			printf("\n");
			deleteCard(&headl, &headr, tempp); //delete the drawn card from the original deck

			/*Inform the player about the Ace's appearance and ask for a desired value */
			if (countAce(headp) > 0) { //if there exists at least an Ace in the hand
				aceAdd = 0;
				printf("\nYou currently have %d Ace(s).\n", countAce(headp)); //inform the player
				for (aceLoop = 1; aceLoop <= countAce(headp); aceLoop++) { //there will be one question for each Ace
					printf("What value of the Ace number %d you want it to be? One (o) or Eleven (e) ", aceLoop); //ask the player the desired value
					scanf(" %c", &o_or_e);
					while ((o_or_e != 'o') && (o_or_e != 'O') && (o_or_e != 'e') && (o_or_e != 'E')) { //assure the answer is in a valid form
						printf("\nOne (o) or Eleven (e) ");
						scanf(" %c", &o_or_e);
					}
					if (o_or_e == 'O' || o_or_e == 'o') { //if the player wants ONE
						aceAdd = aceAdd + 1;
					}
					else { //if the player wants ELEVEN
						aceAdd = aceAdd + 11;
					}
				}
				totalp = handTotalwithoutAce(headp) + aceAdd; //calculate the total value of the hand based on the desired value(s) of the Ace(s)
				if (totalp == 27) { //increment the total of hands that were 27
					numHand_27++;
				}
			}
			else {
				totalp = handTotal(headp); //calculate the total value of the hand normally
				if (totalp == 27) {//increment the total of hands that were 27
				numHand_27++;
				}
			}


			printf("\nPlace your bet: "); // ask for a bet from the player
			scanf("%d", &bet);
			while (bet < 20 || bet >200 || bet > moneyLeft) { //assuring a valid value of the bet
				printf("You have $%d left. You can only bet an amount from $20 to $200 that is within what you have. Please enter again.\n", moneyLeft);
				printf("Place your bet: ");
				scanf("%d", &bet);
			}
			printf("\n");

			int total1 = 0,aceAddd = 0;
			char ansp1, h_or_s = 'a';

			//the first turn of the dealer after two cards being placed
			while (total1 < 14 || (total1 > 14 && total1 < 27)) { //while the total does not satisfy any branch
				tempd = drawCard(headl); //draw another card for dealer from the original deck
				addCard(&headd, &taild, tempd); //add the card to the hand
				total1 = handTotal(headd); //calculate the total for each draw
				deleteCard(&headl, &headr, tempd); //detele the drawn card from the original deck

				if ((tempd->value == 1 || tempd->value == 14) && total1 <= 21) { //if the dealer's drawn card was an Ace, the value of it is different for some certain cases
					aceAddd = aceAddd + 10;
					if (total1+aceAddd == 24) { //if the Ace being 1 makes the dealer reach 14, the Ace will be 1
						aceAddd = 0;
					}
				}
				total1 = total1 + aceAddd; //calculate the dealer's hand total
				if (total1 == 27) {
					numHand_27++;
				}
			}

			//executions based on what the dealer has drawn
			if (total1 == 14) { //if the dealer hits 14, the player has to hit 31 to win the round

				revealCard(headd); //reveal all the cards in the dealer's hand
				printf("Dealer: "); //print dealer's hand
				printList(headd); //print dealer's hand
				numHand++;
				printf("\n");
				printf("Player: ");
				printList(headp); //print player's hand
				numHand++;
				printf("\n");
				printf("\nDealer reaches 14. The player has to hit 31 to win."); //inform the player

				while (totalp < 31) { //the player has to draw a card until reaching 31
					printf("\n\nPress any key to draw a card: ");
					scanf(" %c", &ansp1);
					printf("\n");
					tempp = drawCard(headl); //draw another card for player
					addCard(&headp, &tailp, tempp); //add the card to the player's hand
					deleteCard(&headl, &headr, tempp); //delete that card from the original deck
					printf("Dealer: ");
					printList(headd); //print dealer's hand
					numHand++;
					printf("\n");
					printf("Player: ");
					printList(headp); //print player's hand
					numHand++;

					/*Inform the player about the Ace's appearance. This works similarly as described above*/
					if (countAce(headp) > 0) {
						aceAdd = 0;
						printf("\n\nYou currently have %d Ace(s).\n", countAce(headp));
						for (aceLoop = 1; aceLoop <= countAce(headp); aceLoop++) {
							printf("What value of the Ace number %d you want it to be? One (o) or Eleven (e) ",aceLoop);
							scanf(" %c",&o_or_e);
							while ((o_or_e != 'o') && (o_or_e != 'O') && (o_or_e != 'e') && (o_or_e != 'E')) {
							printf("\nOne (o) or Eleven (e) ");
							scanf(" %c", &o_or_e);
							}
							if (o_or_e == 'O' || o_or_e == 'o') {
								aceAdd = aceAdd + 1;
							}
							else {
								aceAdd = aceAdd + 11;
							}
						}
						totalp = handTotalwithoutAce(headp) + aceAdd;
						if (totalp == 27) {
							numHand_27++;
						}
					}
					else {
						totalp = handTotal(headp);
						if (totalp == 27) {
							numHand_27++;
						}
					}
				}

				/*executions based on what the player has drawn*/
				if (totalp == 31) { //if the player reaches 31, then he wins
					moneyLeft = moneyLeft + bet; //calculate the remaining balance
					printf("\n\nThe player wins. The player wins $%d and now has $%d.\n", bet, moneyLeft);
					printf("The round ends.\n");
					numRound++;
					playerWin++;

					printf("\n============================================\n");
					printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
					printf("Number of games played: %d\n", numGame);
					printf("Number of rounds played in total: %.0lf\n", numRound);
					printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
					printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
					if (playerWin == 0) {
						printf("Percentage of player's win by hitting 14: not found\n");
					}
					else {
						printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
					}
					printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

				}
				else { //if the player goes bust (>31), then he loses
					moneyLeft = moneyLeft - bet; //calculate the remaining balance
					printf("\n\nThe dealer wins. The player loses $%d and now has $%d.\n", bet, moneyLeft);
					printf("The round ends.\n");
					numRound++;
					dealerWin++;

					printf("\n============================================\n");
					printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
					printf("Number of games played: %d\n", numGame);
					printf("Number of rounds played in total: %.0lf\n", numRound);
					printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
					printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
					if (playerWin == 0) {
						printf("Percentage of player's win by hitting 14: not found\n");
					}
					else {
						printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
					}
					printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

				}
			}

			else if (total1 >= 27 && total1 <= 30) { //if the dealer hits somewhere between 27 and 30, then it's the player's turn

				revealCard(headd); //reveal all the cards in the dealer's hand
				tempd->character = 1; //make the last card invisible
				printf("Dealer: ");
				printList(headd); //print dealer's hand
				numHand++;
				printf("\n");
				printf("Player: ");
				printList(headp); //print player's hand
				numHand++;
				printf("\n");

				while ((totalp < 31) && (totalp != 14) && (h_or_s != 's') && (h_or_s != 'S')) { //the player's turn will terminate when the player hits 14, goes over 31, or stands

					while ((h_or_s != 'h') && (h_or_s != 'H') && (h_or_s != 's') && (h_or_s != 'S')) { //assuring a valid answer
						printf("\nHit (h) or Stand (s): ");
						scanf(" %c", &h_or_s);
					}
					printf("\n");
					if ((h_or_s == 'h') || (h_or_s == 'H')) { // if the player hits
						tempp = drawCard(headl); //draw another card for player
						addCard(&headp, &tailp, tempp); //add the card to the player's handdeleteCard(&headl, &headr, tempp); 
						deleteCard(&headl, &headr, tempp); //delete that card from the initial deck	
						h_or_s = 'a';

	
						/*Inform the player about the Ace's appearance. This works similarly as described above*/
						if (countAce(headp) > 0) {
							
							printf("Dealer: "); //print dealer's hand
							printList(headd);
							numHand++;
							printf("\n");
							printf("Player: "); //print player's hand
							printList(headp);
							printf("\n");
							numHand++;
							aceAdd = 0;
							printf("\n\nYou currently have %d Ace(s).\n", countAce(headp));
							for (aceLoop = 1; aceLoop <= countAce(headp); aceLoop++) {
								printf("What value of the Ace number %d you want it to be? One (o) or Eleven (e) ",aceLoop);
								scanf(" %c", &o_or_e);
								while ((o_or_e != 'o') && (o_or_e != 'O') && (o_or_e != 'e') && (o_or_e != 'E')) {
									printf("\nOne (o) or Eleven (e) ");
									scanf(" %c", &o_or_e);
								}
								if (o_or_e == 'O' || o_or_e == 'o') {
									aceAdd = aceAdd + 1;
								}
								else {
									aceAdd = aceAdd + 11;
								}
							}
							totalp = handTotalwithoutAce(headp) + aceAdd;
							if (totalp == 27) {
								numHand_27++;
							}

							if (totalp == 14 || totalp >= 31) { //if the desired value of the Ace(s) makes the player wins, then the program will print out the two hands before informs victory
								revealCard(headd);
								printf("\n");
								printf("Dealer: "); //print dealer's hand
								printList(headd);
								printf("\n");
								printf("Player: "); //print player's hand
								printList(headp);
								printf("\n");
							}
						}

						else {
							totalp = handTotal(headp);
							if (totalp == 27) {
								numHand_27++;
							}
							if (totalp == 14 || totalp >= 31) {
								revealCard(headd);
							}
							printf("Dealer: "); //print dealer's hand
							printList(headd);
							numHand++;
							printf("\n");
							printf("Player: "); //print player's hand
							printList(headp);
							printf("\n");
							numHand++;
						}
					}
				}

				if (totalp == 14 || totalp == 31) { // if the player reaches 14 and the dealer does not have 14 or 31, then the player wins
					
					moneyLeft = moneyLeft + bet;
					printf("\n\nThe player wins. The player wins $%d and now has $%d.\n", bet, moneyLeft);
					printf("The round ends.\n");
					playerWin++;
					numRound++;
					if (totalp == 14) {
						playerWin_14++;
					}

					printf("\n============================================\n");
					printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
					printf("Number of games played: %d\n", numGame);
					printf("Number of rounds played in total: %.0lf\n", numRound);
					printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
					printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
					if (playerWin == 0) {
						printf("Percentage of player's win by hitting 14: not found\n");
					}
					else {
						printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
					}
					printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);
				}

				if (totalp > 31) { // the player loses (goes bust) if total is over 31 
					
					moneyLeft = moneyLeft - bet;
					printf("\n\nThe player loses. The player loses $%d and now has $%d.\n", bet, moneyLeft);
					printf("The round ends.\n");
					numRound++;
					dealerWin++;

					printf("\n============================================\n");
					printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
					printf("Number of games played: %d\n", numGame);
					printf("Number of rounds played in total: %.0lf\n", numRound);
					printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
					printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
					if (playerWin == 0) {
						printf("Percentage of player's win by hitting 14: not found\n");
					}
					else {
						printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
					}
					printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

				}

				if ((h_or_s == 's') || (h_or_s == 'S')) { // if the player stands, the winner will be the one with higher total

					revealCard(headd); //reveal all the cards in the dealer's hand
					printf("Dealer: "); //print dealer's hand
					printList(headd);
					numHand++;
					printf("\n");
					printf("Player: "); //print player's hand
					printList(headp);
					numHand++;

					if (total1 > totalp) { // if the dealer's total is higher
						moneyLeft = moneyLeft - bet;
						printf("\n\nThe dealer wins. The player loses $%d and now has $%d.\n", bet, moneyLeft);
						printf("The round ends.\n");
						dealerWin++;
						numRound++;

						printf("\n============================================\n");
						printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
						printf("Number of games played: %d\n", numGame);
						printf("Number of rounds played in total: %.0lf\n", numRound);
						printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
						printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
						if (playerWin == 0) {
							printf("Percentage of player's win by hitting 14: not found\n");
						}
						else {
							printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
						}
						printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

					}
					else if (totalp > total1) { // if the player's total is higher
						moneyLeft = moneyLeft + bet;
						printf("\n\nThe player wins. The player wins $%d and now has $%d.\n", bet, moneyLeft);
						printf("The round ends.\n");
						playerWin++;
						numRound++;

						printf("\n============================================\n");
						printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
						printf("Number of games played: %d\n", numGame);
						printf("Number of rounds played in total: %.0lf\n", numRound);
						printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
						printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
						if (playerWin == 0) {
							printf("Percentage of player's win by hitting 14: not found\n");
						}
						else {
							printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
						}
						printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

					}
					else { // if both total are equal, then it's a tie (no win no loss)
						printf("\n\nTie. The bet is pushed. Player now has $%d.\n", moneyLeft);
						printf("The round ends.\n");
						numRound++;

						printf("\n============================================\n");
						printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
						printf("Number of games played: %d\n", numGame);
						printf("Number of rounds played in total: %.0lf\n", numRound);
						printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
						printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
						if (playerWin == 0) {
							printf("Percentage of player's win by hitting 14: not found\n");
						}
						else {
							printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
						}
						printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

					}
				}
			}


			else if (total1 == 31) { //if the dealer hits 31, then the dealer wins

				revealCard(headd);
				printf("Dealer: "); //print dealer's hand
				printList(headd);
				numHand++;
				printf("\n");
				printf("Player: "); //print player's hand
				printList(headp);
				numHand++;
				printf("\n");
				moneyLeft = moneyLeft - bet;
				printf("\nThe dealer wins. The player loses $%d and now has $%d.\n", bet, moneyLeft);
				printf("The round ends.\n");
				dealerWin++;
				numRound++;

				printf("\n============================================\n");
				printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
				printf("Number of games played: %d\n", numGame);
				printf("Number of rounds played in total: %.0lf\n", numRound);
				printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
				printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
				if (playerWin == 0) {
					printf("Percentage of player's win by hitting 14: not found\n");
				}
				else {
					printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
				}
				printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

			}

			else { //if the dealer goes bust (>31), then the dealer loses

				revealCard(headd);
				printf("Dealer: "); //print dealer's hand
				printList(headd);
				numHand++;
				printf("\n");
				printf("Player: "); //print player's hand
				printList(headp);
				numHand++;
				printf("\n");
				moneyLeft = moneyLeft + bet;
				printf("\nThe dealer loses. The player wins $%d and now has $%d.\n", bet, moneyLeft);
				printf("The round ends.\n");
				playerWin++;
				numRound++;

				printf("\n============================================\n");
				printf("\nGAME STATS: \n"); // Show the statistics of the game after the user stops playing
				printf("Number of games played: %d\n", numGame);
				printf("Number of rounds played in total: %.0lf\n", numRound);
				printf("Player's winning percentage: %.2lf\%%\n", (playerWin / numRound) * 100);
				printf("Dealer's winning percentage: %.2lf\%%\n", (dealerWin / numRound) * 100);
				if (playerWin == 0) {
					printf("Percentage of player's win by hitting 14: not found\n");
				}
				else {
					printf("Percentage of player's win by hitting 14: %.2lf\%%\n", (playerWin_14 / playerWin) * 100);
				}
				printf("Percentage of hands equal to 27: %.2lf%%\n", (numHand_27 / numHand) * 100);

			}

			deleteList(headd); //delete the dealer's hand after the round ends
			deleteList(headp); //delete the player's hand after the round ends
			headd = NULL; //re-initialize headd
			taild = NULL; //re-initialize taild
			headp = NULL; //re-initialize headp
			tailp = NULL; //re-initialize tailp

			if (moneyLeft <= 0) {  // the game will end when the player runs out of money
				printf("\nThe player has no money left. Game over!\n");
				printf("Do you want to start a new game? Yes (y) or No (n). ");  // Ask the user whether they want to start a new game
				scanf(" %c", &new_game);
				while ((new_game != 'y') && (new_game != 'Y') && (new_game != 'n') && (new_game != 'N')) {
					printf("Do you want to start a new game? Yes (y) or No (n). ");
					scanf(" %c", &new_game);
				}
				if ((new_game == 'n') || (new_game == 'N')) {
					break;
				}
				else {
					printf("\n============================================\n");
					break;
				}
			}
			else if (moneyLeft <  20) { //the game will end also when the player has less than $20 left
				printf("\nYou now have less than $20. Game over!\n");
				new_game = 'a';
				while ((new_game != 'y') && (new_game != 'Y') && (new_game != 'n') && (new_game != 'N')) {
					printf("Do you want to start a new game? Yes (y) or No (n). ");
					scanf(" %c", &new_game);
				}
				if ((new_game == 'n') || (new_game == 'N')) {
					break;
				}
				else {
					printf("\n============================================\n");
					break;
				}
			}
			else { //the player is allowed to proceed to the next round or stop the game
				printf("\nContinue (c) or Stop (s): "); //ask if the player wants to continue or to stop
				scanf(" %c", &s_or_c);
				while ((s_or_c != 'c') && (s_or_c != 'C') && (s_or_c != 's') && (s_or_c != 'S')) {
					printf("\nContinue (c) or Stop (s): ");
					scanf(" %c", &s_or_c);
				}
				printf("\n");
				round++; //increment the number of rounds


				if ((s_or_c == 'S') || (s_or_c == 's')) { //if the player wants to stop
					printf("\nThank you for playing the game. Goodbye!\n");
					exit(0);
				}
			}
		}
	}

	printf("\nThank you for playing the game. Goodbye!\n");

	return 0;
}



void printList(card* h) { // This function prints a deck of card
	while (h != NULL) {
		if (h->suitVal == 1) {
			strcpy(h->suit, "\x06");
		}
		else if (h->suitVal == 2) {
			strcpy(h->suit, "\x03");
		}
		else if (h->suitVal == 3) {
			strcpy(h->suit, "\x04");
		}
		else if (h->suitVal == 4) {
			strcpy(h->suit, "\x05");
		}
		if (h->character == 0) {
			if (h->value == 1 || h->value == 14) {
				printf("A %s, ", h->suit);
			}
			else if (h->value == 11 || h->value == 24) {
				printf("J %s, ", h->suit);
			}
			else if (h->value == 12 || h->value == 25) {
				printf("Q %s, ", h->suit);
			}
			else if (h->value == 13 || h->value == 26) {
				printf("K %s, ", h->suit);
			}
			else if (h->value > 13) {
				printf("%d %s, ", h->value - 13, h->suit);
			}
			else {
				printf("%d %s, ", h->value, h->suit);
			}
		}
		else {
			printf("%c, ", 254);
		}

		h = h->next;
	}
}


void swapCardorder(card* h, int order1, int order2) {
	card* temp1, * temp2, * head;
	temp1 = (card*)malloc(sizeof(card));
	temp2 = (card*)malloc(sizeof(card));
	head = h;
	while (h != NULL) {
		if (h->order == order1) {
			temp1->value = h->value;
			temp1->suitVal = h->suitVal;
			h = h->next;
		}
		else if (h->order == order2) {
			temp2->value = h->value;
			temp2->suitVal = h->suitVal;
			h = h->next;
		}
		else {
			h = h->next;
		}
	}
	h = head;
	while (h != NULL) {
		if (h->order == order1) {
			h->value = temp2->value;
			h->suitVal = temp2->suitVal;
			h = h->next;
		}
		else if (h->order == order2) {
			h->value = temp1->value;
			h->suitVal = temp1->suitVal;
			h = h->next;
		}
		else {
			h = h->next;
		}
	}
}


void swapCard(card* h, int val1, int suit1, int val2, int suit2) { // This function swaps a pair of cards
	while (h != NULL) {
		if (h->value == val1 && h->suitVal == suit1) {
			h->value = val2;
			h->suitVal = suit2;
			h->order = 4 * val2 + suit2 - 4;
			h = h->next;
		}
		else if (h->value == val2 && h->suitVal == suit2) {
			h->value = val1;
			h->suitVal = suit1;
			h->order = 4 * val1 + suit1 - 4;
			h = h->next;
		}
		else {
			h = h->next;
		}
	}
}



card* drawCard(card* h) { // This function draws a card from the original deck
	card* temp, * head;
	int order;
	temp = (card*)malloc(sizeof(card));
	head = (card*)malloc(sizeof(card));
	head = h;
	while (temp->suitVal < 1 || temp->suitVal >4) {
		order = (rand() % 104) + 1;
		while (h != NULL) {
			h->order = 4 * h->value + h->suitVal - 4;
			if (h->order == order) {
				temp->suitVal = h->suitVal;
				temp->value = h->value;
				temp->character = 0;
				h = h->next;
			}
			else {
				h = h->next;
			}
		}
		h = head; //bring h back to the beginning of the list
	}
	return temp;
}



void addCard(card** h, card** t, card* temp) { // This functions adds a card into a hand
	if (*h == NULL) {
		*h = temp;
	}
	else {
		(*t)->next = temp;
	}
	*t = temp;
	temp->next = NULL;
}



void deleteCard(card** hl, card** hr, card* temp) { // This function deletes a card from deck 
	card* curr = *hl;
	while (curr != NULL) {
		if ((curr->value == temp->value) && (curr->suitVal == temp->suitVal)) {
			if (curr->previous == NULL) {
				*hl = curr->next;
			}
			else if (curr->next == NULL) {
				curr->previous->next = NULL;
			}
			else {
				curr->previous->next = curr->next;
				curr->next->previous = curr->previous;
			}
			curr = curr->next;
		}
		else {
			curr = curr->next;
		}
	}
	free(curr);
}



int handTotal(card* h) { //This function calculates the total of a hand
	card* temp = (card*)malloc(sizeof(card));
	int total = 0;
	while (h != NULL) {
		temp->value = h->value;
		if (h->value > 13) {
			temp->value = h->value - 13;
		}
		if (temp->value == 11 || temp->value == 12 || temp->value == 13) {
			temp->value = 10;
		}
		total = total + temp->value;
		h = h->next;
	}
	return total;
}



int countCard(card* h) { //This function counts the number of cards in a certain deck
	int counter = 0;
	while (h != NULL) {
		counter++;
		h = h->next;
	}
	return counter;
}



void deleteList(card* h) { //This function deletes all the cards of a certain list
	card* cur = h, * next;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}
}



void revealCard(card* h) { //This function reveals all the card in the dealer's hand
	while (h != NULL) {
		h->character = 0;
		h = h->next;
	}
}

int countAce(card* h) { //This function counts the number of Ace(s) in a hand
	int counter = 0;
	while (h != NULL) {
		if (h->value == 1 || h->value == 14) {
			counter++;
		}
		h = h->next;
	}
	return counter;
}

int handTotalwithoutAce(card* h) { //This function calculates the total of a hand without the value of Ace
	card* temp = (card*)malloc(sizeof(card));
	int total = 0;
	while (h != NULL) {
		temp->value = h->value;
		if (h->value != 1 && h->value != 14) {
			if (h->value > 13) {
				temp->value = h->value - 13;
			}
			if (temp->value == 11 || temp->value == 12 || temp->value == 13) {
				temp->value = 10;
			}
			total = total + temp->value;
		}
		h = h->next;
	}
	return total;
}
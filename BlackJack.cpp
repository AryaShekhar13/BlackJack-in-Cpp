#include <iostream>
#include <vector>
#include <random>
#include <utility>
#include <algorithm>
#include <fstream>
#include <windows.h>
using namespace std;

void shuffleDeck(vector<pair<string,char>>& deck){
    static mt19937 gen(random_device{}());
    shuffle(deck.begin(), deck.end(), gen);
}

int result;
int wins=0;
int losses=0;
int ties=0;
int total;
int gameNumber = 0;
int insurance=0;
int insurances=0;
int surrenders=0;

void declare_result(int dealer,int user, const vector<pair<string,char>>& userHand, const vector<pair<string,char>>& dealerHand){
    int dealer_score_actual=dealer;
    int user_score=user;
    cout << "Dealer Score: "<< dealer_score_actual << endl;
    cout<<"Your Score: "<<user_score<<endl;
    if(user_score==21 && userHand.size()==2){
    if(dealer_score_actual==21 && dealerHand.size()==2){
        result = 0;
    }
    else{
        result = 2;
        wins++;
    }
    return;
    }
    if(dealer_score_actual<=21){
    if(user_score > 21 ) {
        cout << "You Lose, busted!"<<endl;
        result=-1;
        losses++;
    }
    else if(user_score > dealer_score_actual) {
        cout << "You Win!"<<endl;
        result=1;
        wins++;
    }
    else if(user_score < dealer_score_actual) {
        cout << "You Lose!"<<endl;
        result=-1;
        losses++;
    }else if(user_score == dealer_score_actual){
        cout<<"Push"<<endl;
        result =0;
        ties++;
    }
    }else if(dealer_score_actual>21){
        cout<<"You Win, Dealer Busted!"<<endl;
        result=1;
        wins++;
    }
}

pair<string,char> drawCard(vector<pair<string,char>>& deck, int& j) {
    if (j >= 52) {
        shuffleDeck(deck);
        j = 0;
        ofstream out1("data.txt");
        for (int i=0;i<52;i++) {
        out1 << deck[i].first<<" "<<deck[i].second<< " ";
        }
    }

    return deck[j++];
}

int calculateScore(const vector<pair<string,char>>& hand) {
    int score = 0;
    int aceCount = 0;

    for(size_t i = 0; i < hand.size(); i++) {
        char rank = hand[i].second;

        if(rank == 'A') {
            score += 11;
            aceCount++;
        }
        else if(rank == 'T' || rank == 'J' || rank == 'Q' || rank == 'K') {
            score += 10;
        }
        else {
            score += rank - '0';
        }
    }

    while(score > 21 && aceCount > 0) {
        score -= 10;
        aceCount--;
    }

    return score;
}

bool ins_avail=false;
bool roundOver = false;

char choice_fcn(const vector<pair<string,char>>& Hand, int amount,int total) {
    char choice;

    int m = Hand.size();
    bool pairCards = (m == 2 && Hand[0].second == Hand[1].second);
    bool dd_avail = (m == 2 && 2 * amount <= total);
    bool surrender_avail = (Hand.size() == 2);

    cout << "\nPress 'h' to HIT or 's' to STAND";

    if(pairCards && amount*2<=total) {
        cout << " or 'f' to SPLIT";
    }

    if(dd_avail && amount*2<=total) {
        cout << " or 'd' to DOUBLE DOWN";
    }

    if(surrender_avail) {
        cout << " or 'q' to SURRENDER";
    }

    cout << ": ";
    cin >> choice;

    while(
        choice != 'h' &&
        choice != 's' &&
        !(choice == 'f' && pairCards) &&
        !(choice == 'd' && dd_avail) &&
        !(choice=='q' && surrender_avail)
    ) {
        cout << "Invalid choice. Try again: ";
        cin >> choice;
    }

    return choice;
}

int bet(int& amount, int result, int total) {
    if(result == 1) total += amount;
    else if(result == -1) total -= amount;
    else if(result == 2 ) total += (amount * 3) / 2;
    else if(result== -2)  total -= amount/2;

    return total;
}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    vector<string> suits = {"♥", "♦", "♣", "♠"};
    vector<char> ranks = {'A', '2', '3', '4', '5', '6', '7',
                      '8', '9', 'T', 'J', 'Q', 'K'};

    vector<pair<string,char>> Current_Deck;

    int k=0;
    vector<pair<string,char>> deck(52);
    for(int i=0;i<4;i++){
        for(int j=0;j<13;j++){
        deck[k]={suits[i],ranks[j]};
        k++;
        }
    }

    {
    ifstream walletIn("wallet.txt");
    if (!(walletIn >> total >> wins >> losses >> ties >> gameNumber >> insurances)) {
        total=1000;
        wins=0;
        losses=0;
        ties=0;
        gameNumber=0;
        insurances=0;
        surrenders=0;
    }
    }

    {
    ifstream in("data.txt");
 
    //when file is empty

    if(in.peek() == EOF){

    shuffleDeck(deck);

    ofstream out("data.txt");
    for (int i=0;i<52;i++) {
    out << deck[i].first<<" "<<deck[i].second<< " ";
    }
    }
    }

    //assign file values to the container

    {
    ifstream in1("data.txt");

    string suit;
    char rank;
    while (in1 >> suit >> rank) {
        Current_Deck.push_back({suit, rank});
    }
    }

    //when file is still full(actuall process)

    int user_score=0;
    int dealer_score_actual=0;
    int dealer_score_show=0;

    int j=0;
    {
    ifstream a("Score.txt");
    if(!(a >> j)) {
        j = 0;
    }
    }
    if(j < 0 || j >= 52) {
        j = 0;
    }
    vector<pair<string,char>> userHand;
    vector<pair<string,char>> dealerHand;

    cout<<"Enter your wager:"<<endl;
    int amount;
    cin>>amount;
    while(amount>total || amount<=0 ){
        cout<<"Please enter a valid number below wallet balance: "<<total<<endl;
        cin>>amount;
    }
    
    pair<string, char> dealerShown = drawCard(Current_Deck, j);
    dealerHand.push_back(dealerShown);
    dealer_score_show = calculateScore(dealerHand);

    dealerHand.push_back(drawCard(Current_Deck, j));

    pair<string, char> userCard1 = drawCard(Current_Deck, j);
    userHand.push_back(userCard1);
    pair<string, char> userCard2 = drawCard(Current_Deck, j);
    userHand.push_back(userCard2);


    cout << "Dealers Card: " << dealerShown.first << dealerShown.second << endl;
    cout << "Your Cards: "<< userCard1.first << userCard1.second << " "<< userCard2.first << userCard2.second << endl;

    dealer_score_actual = calculateScore(dealerHand);
    user_score = calculateScore(userHand);

    char ins_ch;

     if(dealer_score_show==11){
        cout<<"Dealer's Score: "<<dealer_score_show<<" / "<<dealer_score_show-10<<endl;
    }
    else {
    cout << "Dealer's Score: " << dealer_score_show << endl;
    }
    cout<<"Your Score: "<<user_score<<endl;

    if(dealerHand[0].second=='A'){

        if(total >= amount + amount/2) {
        cout<<"Press 'y' for INSURANCE and 'n' for NO-INSURANCE:"<<endl;
        cin>>ins_ch;
        while(ins_ch!='y' && ins_ch!='n'){
            cout<<"Try 'y' or 'n'"<<endl;
            cin>>ins_ch;     
        }
        if(ins_ch=='y'){
        ins_avail=true;
        insurance=amount/2;
        insurances++;
        }
        if(ins_ch=='n') ins_avail=false;   
        }   
    }

    bool dealerBJ = (dealer_score_actual == 21 && dealerHand.size() == 2);
    bool userBJ   = (user_score == 21 && userHand.size() == 2);

    if(dealerBJ) {

    if(userBJ) {
        cout << "Push" << endl;
        ties++;
        result = 0;
    }
    else {
        cout << "You Lose!" << endl;
        losses++;
        result = -1;
    }

    if(ins_avail) {
        total+=2*insurance;
    }

    // resolve hand
    roundOver = true;
    }
    else {

    if(ins_avail) {
    // lose insurance
        total -= insurance;
    }
    }

    if(userBJ && !dealerBJ) {
    cout << "Blackjack! You Win!\n";
    result = 2;
    wins++;
    roundOver = true;
    }
    
    //choice protocol

    int i=0;

    while(user_score < 21 && roundOver==false) {

    char choice = choice_fcn(userHand,amount,total);

    if(choice == 'h') {

        pair<string, char> newCard = drawCard(Current_Deck, j);
        userHand.push_back(newCard);
        user_score = calculateScore(userHand);

        cout << "New Card: " << newCard.first << newCard.second << endl;
        cout << "Your Score: " << user_score << endl;
    }else if(choice=='d'){

        amount*=2;

        pair<string, char> newCard = drawCard(Current_Deck, j);
        userHand.push_back(newCard);
        user_score = calculateScore(userHand);

        cout << "New Card: " << newCard.first << newCard.second << endl;
        cout << "Your Score: " << user_score << endl;
        break;
    }else if(choice == 's') {

    break;

    }else if(choice == 'q'){

    result = -2;
    roundOver = true;
    break;

    }else if(choice=='f'){

    }

    }  
    
    if(user_score<=21 && roundOver==false){
    
        while(dealer_score_actual<=16){
        auto card = drawCard(Current_Deck,j);
        dealerHand.push_back(card);
        
       dealer_score_actual = calculateScore(dealerHand);

        }
    }

    cout<<"Dealer's Final Hand: "<<endl;
    for(size_t i=0;i<dealerHand.size();i++){
        cout<<dealerHand[i].first<<dealerHand[i].second<<" ";
    }
    cout<<endl;

    //wallet refresh and wins/losses

    if(roundOver!=true) declare_result(dealer_score_actual, user_score, userHand,dealerHand);

    gameNumber++;

    total = bet(amount, result, total);

    cout << "Total Balance: " << total << endl;

    //rewriting values in score
    ofstream b("Score.txt", ios::trunc);
    b << j;

    ofstream graph("graph.csv", ios::app);
    graph << gameNumber << "," << total << "\n";

    if(total <= 0){
    total = 1000;
    }

    ofstream walletOut("wallet.txt", ios::trunc);
    walletOut << total << " " << wins << " " << losses << " " << ties << " " << gameNumber<<" "<<insurances<<" "<<surrenders;
}
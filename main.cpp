#include <iostream>
#include <algorithm>
#include <math.h>
#define MODE_DEB     0
#define MODE_TEST    1
#define PRINT_RAPORT 0

// PUNKT PRZECIĘCIA LEŻY W STARCIE STONOGI!
// DETERMINE COLLISIONTIMES

#define TEST_PASS   SetConsoleTextAttribute(hConsole,10);cout<<"PASS";SetConsoleTextAttribute(hConsole,7);
#define TEST_FAIL   SetConsoleTextAttribute(hConsole,12);cout<<"FAIL";SetConsoleTextAttribute(hConsole,7);

#if MODE_TEST == 1
#include <windows.h>
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif // MODE_TEST

#if PRINT_RAPORT == 1
#include /printraporttohtml.cpp
#endif // PRINT_RAPORT

// TODO
// PUNKT PRZECIĘCIE WEWNĄTRZ POZYCJI STARTOWEJ
// speed = 0;
// proste pokrywające się zderzenie dczołowe

using namespace std;

enum direction
{
    neg,    // Negative direction
    pos     // Positive direction
};

enum collision
{
    collisionNO,
    collisionYES
};

enum intersection
{
    intersection_NO,
    intersection_ONE,
    intersection_INF
};

enum TimesOverleap
{
    overleapExist_NO,
    overleapExist_YES
};

enum BelongsToInitial
{

    CrossPointBelongsToInitial_FALSE,
    CrossPointBelongsToInitial__TRUE
};


struct POIN
{
    double x;
    double y;
};

struct PLZ
{
    int Min;
    int Max;
}
PlayZone {100000,-100000};

// 0 = Ax - By + F
struct LINFAC
{
    double A;
    double B;
    double C;
};

struct MILLPEDE
{
    POIN    P1;
    POIN    P2;
    double  speed;
    LINFAC  lin;
    string  CrossPointExist;
    POIN    CrossPoint;
    int     CPdirection;
    int     CPbelongsToInitial;
    double  CrossTimeStart;
    double  CrossTimeEnd;
    int     TimesOverleap;
};


int  CHECK_MillipedeCollision(MILLPEDE *ST1, MILLPEDE *ST2);
void DETERMINE_LineEquation(MILLPEDE *ST);
void DETERMINE_CrossPoint(MILLPEDE *T1, MILLPEDE *T2);
void DETERMINE_CrossDirection(MILLPEDE *T1);
void DETERMINE_CrossTimes(MILLPEDE *T1);
void DETERMINE_Overleap(MILLPEDE *T1, MILLPEDE *T2);
int  CHECK_IF_CollisionPointbelongstoPlayZone(MILLPEDE *T, PLZ PlayZone);
int  CHECK_IF_CrossPointBelongsToMillipedeForInitial(MILLPEDE *T1);



int TEST__CheckMillipedeCollision(void);
int TEST__DETERMINE_LineEquation(void);
int TEST__DETERMINE_CrossPoint(void);
int TEST__DETERMINE_CrossDirection(void);
int TEST__DETERMINE_CrossTimes(void);
int TEST__DETERMINE_Overleap(void);
int TEST__CHECK_IF_CollisionPointbelongstoPlayZone(void);
int TEST__CHECK_IF_CrossPointBelongsToMillipedeForInitial(void);



void LoadData(MILLPEDE *ST1, MILLPEDE *ST2);
void TerminalSettings(void);
void TestScenarioSettings(void);


int main()
{
    TerminalSettings();
    TestScenarioSettings();

    MILLPEDE ST1;
    MILLPEDE ST2;

    int Cases;
    cin >> Cases;
    while(--Cases)
    {
        LoadData(&ST1, &ST2);
        CHECK_MillipedeCollision(&ST1, &ST2);
    }

    return 0;
}



void LoadData(MILLPEDE *ST1, MILLPEDE *ST2)
{
    cin >> ST1->P1.x
        >> ST1->P1.y
        >> ST1->P2.x
        >> ST1->P2.y
        >> ST1->speed;

    cin >> ST2->P1.x
        >> ST2->P1.y
        >> ST2->P2.x
        >> ST2->P2.y
        >> ST2->speed;
}

int CHECK_MillipedeCollision(MILLPEDE *ST1, MILLPEDE *ST2)
{

    DETERMINE_LineEquation(ST1); // Determine line eqution for ST!
    DETERMINE_LineEquation(ST2); // Determine line eqution for ST2

    if
        (
         ST1->lin.A == ST2->lin.A
         &&
         ST1->lin.B == ST2->lin.B
         &&
         ST1->lin.C == ST2->lin.C
         )
    {
        cout << "Case in progress";
    }
    else if
        (
         ST1->lin.A == ST2->lin.A
         &&
         ST1->lin.B == ST2->lin.B
         &&
         ST1->lin.C != ST2->lin.C
         )
    {
        return collisionNO;
    }
    else
    {

        DETERMINE_CrossPoint(ST1,ST2);

        if(ST1->CrossPointExist == "YES")
        {

            CHECK_IF_CrossPointBelongsToMillipedeForInitial(ST1);
            if
            (
            CrossPointBelongsToInitial_FALSE
            ==
            ST1->CPbelongsToInitial
            )
            {
                DETERMINE_CrossDirection(ST1);
                if(neg == ST1->CPdirection ) return collisionNO;
            }


            CHECK_IF_CrossPointBelongsToMillipedeForInitial(ST2);
            if
            (
            CrossPointBelongsToInitial_FALSE
            ==
            ST2->CPbelongsToInitial
            )
            {
                DETERMINE_CrossDirection(ST2);
                if(neg == ST2->CPdirection ) return collisionNO;
            }

            DETERMINE_CrossTimes(ST1);
            DETERMINE_CrossTimes(ST2);

            DETERMINE_Overleap(ST1,ST2);

            if(overleapExist_YES == ST1->TimesOverleap)
                    return collisionYES;
            else
                    return collisionNO;
        }
    else return collisionNO;
    }
    return -1;
}

void DETERMINE_LineEquation(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "                     "  << endl;
    cout << "DetermineLineEquation";
    #endif // MODE_DEB



    // LINIA PIONOWA
    if(T->P1.x == T->P2.x)
    {
        T->lin.A =  1;
        T->lin.B =  0;
        T->lin.C = -T->P1.x;
    }
    // LINIA POZIOMA
    else if(T->P1.y == T->P2.y)
    {
        T->lin.A = 0;
        T->lin.B = 1;
        T->lin.C = -T->P1.y;

    }
    else
    {

        double Slope;
        double Intercept;

    //
    // SLOPE // Ax
    //

        if(0 == (max(T->P1.y, T->P2.y) - min(T->P1.y, T->P2.y))) Slope = 0;
        else Slope = (T->P2.y - T->P1.y) / (T->P2.x - T->P1.x);
        T->lin.A = Slope;

    //
    // INTERCEPT // F
    //

        if(0 == Slope) Intercept = T->P1.x;
        else Intercept = T->P1.y - Slope * T->P1.x;
        T->lin.C = Intercept;

    //
    // By
    //

        T->lin.B = -1;

    }


    #if MODE_DEB == 1
    cout << " >> (A,B,C): " <<
    "("  << T->lin.A <<
    ","  << T->lin.B <<
    ","  << T->lin.C <<
    ")"  << endl;
    //cout << "B_: " << T->P1.y <<"-"<< T->lin.A <<"*"<< T->P1.x<< endl;
    #endif // MODE_DEB
}

int DETERMIEN_IntersectionScenario(MILLPEDE *ST1, MILLPEDE *ST2)
{
    return 0;
}

//
//
//
// https://www.geeksforgeeks.org/point-of-intersection-of-two-lines-formula/
//
//
//
void DETERMINE_CrossPoint(MILLPEDE *T1, MILLPEDE *T2)
{
    #if MODE_DEB == 1
    cout << "                   " << endl;
    cout << "DetermineCrossPoint";
    #endif // MODE_DEB

    POIN CP;

    if(T1->lin.A == T2->lin.A)
    {
        T1->CrossPointExist = "NO";
        T2->CrossPointExist = "NO";
    }
    else
    {
        T1->CrossPointExist = "YES";
        T2->CrossPointExist = "YES";

    CP.x =
        (T1->lin.B * T2->lin.C - T2->lin.B * T1->lin.C)
                               /
        (T1->lin.A *T2->lin.B  - T2->lin.A * T1->lin.B );


    CP.y =
        (T2->lin.A * T1->lin.C - T2->lin.C * T1->lin.A )
                               /
                    (T2->lin.A - T1->lin.A);

    CP.y =
        (T1->lin.C * T2->lin.A - T2->lin.C * T1->lin.A)
                               /
        (T1->lin.A *T2->lin.B  - T2->lin.A * T1->lin.B );

    T1->CrossPoint.x = CP.x;
    T1->CrossPoint.y = CP.y;
    T2->CrossPoint.x = CP.x;
    T2->CrossPoint.y = CP.y;

    }
    #if MODE_DEB == 1
    cout << " >> CP: (" << T1->CrossPoint.x <<","<<T1->CrossPoint.y <<")" << endl << endl;
    #endif // MODE_DEB
}



int CHECK_IF_CollisionPointbelongstoPlayZone(MILLPEDE *T, PLZ PlayZone)
{
    return -1;
}


int CHECK_IF_CrossPointBelongsToMillipedeForInitial(MILLPEDE *T)
{
    if
    (
        max(T->P1.x,T->P2.x) >= T->CrossPoint.x && T->CrossPoint.x >= min(T->P1.x,T->P2.x)
        &&
        max(T->P1.y,T->P2.y) >= T->CrossPoint.y && T->CrossPoint.y >= min(T->P1.y,T->P2.y)
    )
         return T->CPbelongsToInitial = CrossPointBelongsToInitial__TRUE;
    else return T->CPbelongsToInitial = CrossPointBelongsToInitial_FALSE;
}



// trzeba policzyc ktory z punktow P1,P2 jest blizej punktu CP,
// jeżeli punkt P1 jest bliżej CP to kierunek jset pozytywny
// jeż�eli punkt P2 jest bliżej CP to kierunek jest negatywny
//
// Examples:
// P1<------P2      CP      // DIRECTION NEGATIVE
// P2------>P1      CP      // DIRECTION POSITIVE
// CP    P1<--------P2      // DIRECTION POSITIVE
// CP    P2-------->P1      // DIRECTION NEGATIVE

void DETERMINE_CrossDirection(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "                       " << endl;
    cout << "DetermineCrossDirection";
    #endif // MODE_DEB

    double cpp1,cpp2;
    cpp1 = sqrt(pow(T->P1.x - T->CrossPoint.x,2) + pow(T->P1.y - T->CrossPoint.y,2));
    cpp2 = sqrt(pow(T->P2.x - T->CrossPoint.x,2) + pow(T->P2.y - T->CrossPoint.y,2));

    if      (cpp1 == cpp2) cout << endl << "FATAL ERROR" << endl;
    else if (cpp1 <  cpp2) T->CPdirection = pos;
    else if (cpp1 >  cpp2) T->CPdirection = neg;
    else                   cout << endl << "FATAL ERROR" << endl;


    #if MODE_DEB == 1
    cout << " >> Direction: ";
    if      (pos == T->CPdirection) cout << "positive"       << endl;
    else if (neg == T->CPdirection) cout << "negative"       << endl;
    else                           cout << "FATAL ERRor"    << endl;
    #endif // MODE_DEB
}

void DETERMINE_CrossTimes(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "                   "  << endl;
    cout << "DetermineCrossTimes"  << endl;
    #endif // MODE_DEB

    // t=s/v
    double S1;
    double S2;

    #if MODE_DEB == 1
    //cout << "DEBOUG DATA: " << endl;
    //cout << "P1:   (" << T->P1.x         <<","<< T->P1.y         << ")" << endl;
    //cout << "P2:   (" << T->P2.x         <<","<< T->P2.y         << ")" << endl;
    //cout << "CP:   (" << T->CrossPoint.x <<","<< T->CrossPoint.y << ")" << endl;
    //cout << "Speed: " << T->speed << endl;
    #endif // MODE_DEB

    S1 = 

    sqrt(
            pow(T->P1.x - T->CrossPoint.x ,2)
            +
            pow(T->P1.y - T->CrossPoint.y ,2)
        );

    S2 =   sqrt
           (
           pow(T->P2.x - T->CrossPoint.x ,2)
           +
           pow(T->P2.y - T->CrossPoint.y ,2)
           );

    if(S1<S2)
    {
        T->CrossTimeStart = S1/T->speed;
        T->CrossTimeEnd   = S2/T->speed;
    }
    else if(S1>S2)
    {
        T->CrossTimeStart = S2/T->speed;
        T->CrossTimeEnd   = S1/T->speed;
    }
    else
    {
        cout << "Fatal Error - DetermineCrossTimes()";
    }

    if(CrossPointBelongsToInitial__TRUE == T->CPbelongsToInitial)
    {
        T->CrossTimeStart = 0;
        T->CrossTimeEnd   = S2/T->speed;
    }


    //TODO
    // SPEED == 0
    // PUNKT PRZECIĘCIA LEŻY W STARCIE STONOGI!
    #if MODE_DEB == 1
    cout << "-----------------------------"  << endl;
    cout << "P1: (" << T->P1.x << "," << T->P1.y << ")" << endl;
    cout << "P2: (" << T->P2.x << "," << T->P2.y << ")" << endl;
    cout << "S1: " << S1 << endl;
    cout << "S2: " << S2 << endl;
    if(S1>S2)
    {
    cout << "CrossTimeStart:          >> "  << T->CrossTimeStart << endl;
    cout << "Calc: " << S2<<"/"<<T->speed   << endl;
    cout << "CrossTimeEnd:            >> "  << T->CrossTimeEnd << endl;
    cout << "Calc: " << S1<<"/"<<T->speed   << endl;
    }
    if(S1<S2)
    {
    cout << "CrossTimeStart:          >> "  <<T->CrossTimeStart << endl;
    cout << "Calc: " << S1<<"/" <<T->speed          << endl;
    cout << "CrossTimeEnd:            >> "  <<T->CrossTimeEnd   << endl;
    cout << "Calc: " << S2<<"/" <<T->speed          << endl;
    }
    cout << "-----------------------------"  << endl;
    cout << endl;
    #endif // MODE_DEB
}


void DETERMINE_Overleap(MILLPEDE *T1, MILLPEDE *T2)
{
    #if MODE_DEB == 1
    cout << "                 " << endl;
    cout << "DetermineOverleap";
    #endif // MODE_DEB

    if(T1->CrossTimeStart < T2->CrossTimeStart)
    {
        if      (T1->CrossTimeEnd < T2->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_NO;
            T2->TimesOverleap = overleapExist_NO;
        }
        else if (T1->CrossTimeEnd > T2->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;
        }
        else
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;
        }

    }
    else if(T2->CrossTimeStart < T1->CrossTimeStart)
    {
        if (T2->CrossTimeEnd < T1->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_NO;
            T2->TimesOverleap = overleapExist_NO;
        }
        else if(T2->CrossTimeEnd > T1->CrossTimeStart)
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;
        }
        else
        {
            T1->TimesOverleap = overleapExist_YES;
            T2->TimesOverleap = overleapExist_YES;
        }


    }
    else
    {
        T1->TimesOverleap = overleapExist_YES;
        T2->TimesOverleap = overleapExist_YES;
    }

    #if MODE_DEB == 1
    if(overleapExist_YES == T1->TimesOverleap)
        cout << " >> overleap_YES" << endl;
    if(overleapExist_NO == T1->TimesOverleap)
        cout << " >> overleap_NO" << endl;
        cout << endl;
    #endif // MODE_DEB

}

//
//===============================================================
// TEST FUNCTIONS
//===============================================================
//



void TerminalSettings(void){

    #if MODE_TEST == 1

    cout << "=============================" << endl;
    cout << "         MODE_TEST  =1     " << endl;
    cout << "=============================" << endl;
    #endif // MODE_TEST
    #if MODE_DEB == 1
    cout << "=============================" << endl;
    cout << "         MODE_DEB  =1     " << endl;
    cout << "=============================" << endl;
    #endif // MODE_DEB
}



void TestScenarioSettings()
{
    #if MODE_TEST == 1
    TEST__DETERMINE_LineEquation();
    TEST__DETERMINE_CrossPoint();
    TEST__DETERMINE_CrossDirection();
    TEST__DETERMINE_CrossTimes();
    TEST__DETERMINE_Overleap();
    TEST__CHECK_IF_CrossPointBelongsToMillipedeForInitial();
    TEST__CHECK_IF_CollisionPointbelongstoPlayZone();


    TEST__CheckMillipedeCollision();

    #endif // MODE_TEST
}



int TEST__DETERMINE_LineEquation( void )
{

    int AmountOfTest = 11;
    struct TESTTable
{
    POIN P1;
    POIN P2;
    LINFAC expected;
};

    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> Determine line equation TESTS" << endl;
    cout << " >>> " << endl;
    cout << endl;

    TESTTable TT[AmountOfTest] = {

	// linia pionowa
    //----------------| |-----------------|
    //        |       | | Expected LINFAC |
    //  P1    |  P2   | |  A  |  B  |  C  |
    //        |       | |     |     |     |
          0, 2,   0, 4,      1,    0,    0,
         10, 0,  10,10,      1,    0,  -10,
          0, 4,   0, 2,      1,    0,    0,
          3, 4,   3,-2,      1,    0,   -3,

	// linia pozioma
    //-----------------| |-----------------|
    //        |        | | Expected LINFAC |
    //   P1   |   P2   | |  A  |  B  |  C  |
    //        |        | |     |     |     |
        2, 0  ,  4, 0  ,      0,    1,    0,
        2,10  ,  4,10  ,      0,    1,  -10,
        2,-5  ,  4,-5  ,      0,    1,    5,

	// linia skosna
    //----------------| |-------------------------------|
    //        |       | | Expected LINFAC               |
    //  P1    |  P2   | |      A      | B |      C      |
    //        |       | |             |   |             |
         -1,-2, 1, 2,                2, -1,            0,
         -4,-2,-1,-4,     (double)-2/3, -1,(double)-14/3,
         -1, 2, 9, 0,     (double)-1/5, -1,(double)  9/5,
          1, 1, 2, 2,                1, -1,            0
    };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL = {0,0,0,0,0,0,0};
        MILL.P1 = TT[i].P1;
        MILL.P2 = TT[i].P2;

        DETERMINE_LineEquation(&MILL);

        cout << "TC" << i+1 << ": ";
        if
        (
             (fabs(MILL.lin.A - TT[i].expected.A) <= 1e-10)
             &&
             (fabs(MILL.lin.C - TT[i].expected.C) <= 1e-10)
             &&
             (fabs(MILL.lin.B - TT[i].expected.B) <= 1e-10)
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }


        cout << "  ||  ";
        cout << "P1("<<TT[i].P1.x<<","<<TT[i].P1.y<<")";
        cout << "P2("<<TT[i].P2.x<<","<<TT[i].P2.y<<")";
        cout << "    ";

        cout << "(A,B,C): EXP: ("
             << TT[i].expected.A << ","
             << TT[i].expected.B << ","
             << TT[i].expected.C << ")";
        cout << "  OBS: ("
             << MILL.lin.A << ","
             << MILL.lin.B << ","
             << MILL.lin.C << ")" << endl;
    }
    return 0;
}



int TEST__DETERMINE_CrossPoint(void)
{

    // 0 = ST2A * X + ST2B + Y

    struct TESTTable
    {
    POIN    S1P1;
    POIN    S1P2;
    POIN    S2P1;
    POIN    S2P2;
    string  Expected_CrossPointExist;
    POIN    Expected_CP;
    };

    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> Determine cross point TESTS" << endl;
    cout << " >>> " << endl;
    cout << endl;

    int AmountOfTest = 11;
    TESTTable TT[AmountOfTest] =
    {
    //----------------|---------------|------|--------|
    //ST1             |ST2            |CP    |CP      |
    //P1      |P2     |P1     |P2     |EXIST |Expect  |
    //        |       |       |       |      |        |
          0, 2,   0, 4,  2,  0,  4,  0, "YES",   0,   0,
         -2, 0,  -4, 0,  0, -2,  0, -4, "YES",   0,   0,
         -1, 1,  -2, 2,  1,  1,  2,  2, "YES",   0,   0,
         -1, 2,  -2, 3,  1,  2,  2,  3, "YES",   0,   1,
          0, 1,  -1, 1,  1,  0,  1, -1, "YES",   1,   1,

          1, 2,   0, 2,  2,  1,  2,  0, "YES",   2,   2,
          1, 2,  -1, 2,  2,  1,  2, -1, "YES",   2,   2,
         -4, 2,  -6, 2, -2,  0, -2, -2, "YES",  -2,   2,
         -2,-1,  -2, 1, -1, -2,  1, -2, "YES",  -2,  -2,

          0, 0,  -1, 1,  0,  1,  1,  2, "YES",-0.5, 0.5,
          0, 1,   1, 1,  0,  2,  1,  2, "NO" , 'X', 'X'
    };

    for(int i=0; i<AmountOfTest; i++)
    {

        #if MODE_DEB == 1
        cout <<                            endl;
        cout <<                            endl;
        cout << "    ST1(P1)(P2) >> "
             << "(" << TT[i].S1P1.x << "," << TT[i].S1P1.y << ")"
             << "(" << TT[i].S1P2.x << "," << TT[i].S1P2.y << ")" << endl;
        cout << "    ST2(P1)(P2) >> "
             << "(" << TT[i].S2P1.x << "," << TT[i].S2P1.y << ")"
             << "(" << TT[i].S2P2.x << "," << TT[i].S2P2.y << ")" << endl;
        cout << "    ------------------------------" << endl;
        #endif // MODE_DEB

        MILLPEDE MILL1;
        MILLPEDE MILL2;

        MILL1.P1 = TT[i].S1P1;
        MILL1.P2 = TT[i].S1P2;
        MILL2.P1 = TT[i].S2P1;
        MILL2.P2 = TT[i].S2P2;

        DETERMINE_LineEquation(&MILL1);
        DETERMINE_LineEquation(&MILL2);
        DETERMINE_CrossPoint(&MILL1, &MILL2);

        cout << "TC" << i+1 << ": ";

        if
        (("YES" == TT[i].Expected_CrossPointExist)
         &&
         (fabs(MILL1.CrossPoint.x - TT[i].Expected_CP.x) <=1e-10)
         &&
         (fabs(MILL1.CrossPoint.y - TT[i].Expected_CP.y) <=1e-10)
        )
        {
            TEST_PASS;
        }
        else if
        ("NO" == TT[i].Expected_CrossPointExist)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }

        cout << " || ";
        cout << MILL1.CrossPointExist << "?>" << TT[i].Expected_CrossPointExist  << "<";
        cout << " || ";
        cout << "exp CP: ("<< TT[i].Expected_CP.x<<","<<TT[i].Expected_CP.y<<")";
        cout << " || ";
        cout << "obs CP: ("<<MILL1.CrossPoint.x  <<","<<MILL1.CrossPoint.y <<")";
        cout << endl;

        #if MODE_DEB == 1
        //cout << "x:      "<<MILL1.CrossPoint.x << TT[i].Expected_CP.x << endl;
        //cout << "logicx: "<<(fabs(MILL1.CrossPoint.x    - TT[i].Expected_CP.x) <=1e-10) << endl;
        //cout << "fabs:   "<<fabs(MILL1.CrossPoint.y    - TT[i].Expected_CP.y) << endl;
        //cout << "y:      "<<MILL1.CrossPoint.y << TT[i].Expected_CP.y << endl;
        //cout << "logicx: "<< (fabs(MILL1.CrossPoint.x    - TT[i].Expected_CP.x) <=1e-10) << endl;
        #endif // MODE_DEB


    }
    return 0;
}

int TEST__CHECK_IF_CollisionPointbelongstoPlayZone(void)
{
    return -1;
}



int TEST__CHECK_IF_CrossPointBelongsToMillipedeForInitial(void)
{
    struct TESTTable
    {
    POIN P1;
    POIN P2;
    POIN CP;
    int expected_result;
    };


    cout <<                                                                         endl;
    cout << " >>> "                                                             <<  endl;
    cout << " >>> TEST FOR: CheckIfCrossPointBelongsToMillipedeForInitial() "   <<  endl;
    cout << " >>> "                                                             <<  endl;
    cout <<                                                                         endl;


    int AmountOfTest = 6;
    TESTTable TT[AmountOfTest] =
    {

    //|--------|--------|--------|----------------------------------|
    //|   P1   |   P2   |   CP   |           Expected result        |
    //|        |        |        |                                  |
          0,  0,   0,  6,   0,  3, CrossPointBelongsToInitial__TRUE ,
          0,  0,   6,  0,   3,  0, CrossPointBelongsToInitial__TRUE ,
          0,  0,   0,  6,   0, 10, CrossPointBelongsToInitial_FALSE ,
          0,  0,   6,  0,  10,  0, CrossPointBelongsToInitial_FALSE ,
          0,  0,   1,  1, 100,100, CrossPointBelongsToInitial_FALSE ,
         -1, -1,   1, -1,   0, -1, CrossPointBelongsToInitial__TRUE
    };

    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;

        MILL.P1          = TT[i].P1;
        MILL.P2          = TT[i].P2;
        MILL.CrossPoint  = TT[i].CP;


        CHECK_IF_CrossPointBelongsToMillipedeForInitial(&MILL);


        cout << "TC" <<i<<": ";


        if(MILL.CPbelongsToInitial == TT[i].expected_result)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }
        cout << endl;
    }
    return -1;
}



int TEST__DETERMINE_CrossDirection(void)
{
   struct TESTTable
    {
    POIN P1;
    POIN P2;
    POIN CP;
    int expected_direction;
    };


    cout <<                                                 endl;
    cout << " >>> "                                      << endl;
    cout << " >>> TEST FOR: DetermineCrossDirection() "  << endl;
    cout << " >>> "                                      << endl;
    cout <<                                                 endl;


    int AmountOfTest = 10;
    TESTTable TT[AmountOfTest] =
    {
    //|----------|----------|------------------|-----------------|
    //|    P1    |    P2    |        CP        | Expected result |
    //|          |          |                  |                 |
            -1, 2,      9, 0,       10,    -0.2,    neg          ,
            -1, 2,      9, 0,       20,   9/5-4,    neg          ,
            -1, 2,      9, 0,      -20,   9/5+4,    pos          ,
             1, 1,      2, 2,        0,       0,    pos          ,
             1, 1,      2, 2,     -111,    -111,    pos          ,
             1, 1,      2, 2,   -60000,  -60000,    pos          ,
             1, 1,      2, 2,        3,       3,    neg          ,
            -1,-2,      1, 2,        2,       4,    neg          ,
            -1,-2,      1, 2,       -2,      -4,    pos          ,
            -4, 0,     -2,-2,-0.818182,-3.18182,    neg
    };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;

        MILL.P1.x          = TT[i].P1.x;
        MILL.P1.y          = TT[i].P1.y;
        MILL.P2.x          = TT[i].P2.x;
        MILL.P2.y          = TT[i].P2.y;
        MILL.CrossPoint.x  = TT[i].CP.x;
        MILL.CrossPoint.y  = TT[i].CP.y;

        DETERMINE_CrossDirection(&MILL);

        cout << "TC" <<i<<": ";

        if(MILL.CPdirection == TT[i].expected_direction)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }

        cout <<"  ||  ";
        cout <<"("<<MILL.P1.x<<","<<MILL.P1.y<<") ";
        cout <<"("<<MILL.P2.x<<","<<MILL.P2.y<<") ";
        cout <<"("<<MILL.CrossPoint.x<<","<<MILL.CrossPoint.y<<") ";
        cout <<"  ||";
        cout <<"  exp: " << TT[i].expected_direction;
        cout <<"  obs: " << MILL.CPdirection;
        cout << endl;

    }
    return -1;
}
int TEST__DETERMINE_CrossTimes(void)
{
    struct TESTTable
    {
    POIN    P1;
    POIN    P2;
    double  speed;
    POIN    CP;
    int     CPbelongsToInitial;
    double  expected_CrossStart;
    double  expected_CrossEnd;
    };


    int AmountOfTest = 6;
    TESTTable TT[AmountOfTest] =
    {
    //|----------|----------|------------------|--------------|---------------------------------|exp  |exp  |
    //|    P1    |    P2    |        speed     |      CP      |    CPbelongsToInitial result    |cross|cross|
    //|          |          |                  |              |                                 |start|end  |
          1,    0,    0,   0,                 1,        2,   0, CrossPointBelongsToInitial_FALSE,    1,    2,
         -1,   -2,    1,   2,   (double)sqrt(5),        2,   4, CrossPointBelongsToInitial_FALSE,    1,    3,
          4,    3,    4,  -1,                 2,        4,   5, CrossPointBelongsToInitial_FALSE,    1,    3,
          3,    2,    1,   2,                 1,        4,   2, CrossPointBelongsToInitial_FALSE,    1,    3,
         -2,    2,   -6,  -2,   (double)sqrt(8),        0,   4, CrossPointBelongsToInitial_FALSE,    1,    3,
        100,    0,  200,   0,                 1,        0,   0, CrossPointBelongsToInitial_FALSE,  100,  200
    };


    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> TEST FOR: DetermineCrossTimes() " << endl;
    cout << " >>> " << endl;
    cout << endl;


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;
        MILL.P1                 = TT[i].P1;
        MILL.P2                 = TT[i].P2;
        MILL.CrossPoint         = TT[i].CP;
        MILL.speed              = TT[i].speed;
        MILL.CPbelongsToInitial = TT[i].CPbelongsToInitial;

        DETERMINE_CrossTimes(&MILL);

        if(i+1<10)cout << "TC" <<i+1<<" : ";
        else cout << "TC" <<i+1<<": ";

        if
        (
            fabs(MILL.CrossTimeStart - TT[i].expected_CrossStart) <=1e-5
            &&
            fabs(MILL.CrossTimeEnd   - TT[i].expected_CrossEnd) <=1e-5
        )
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }


        cout << "  ||  ";
        cout << "exp START: "   << TT[i].expected_CrossStart << "  ";
        cout << "obs START: "   << MILL.CrossTimeStart       << " || ";
        cout << "exp END: "     << TT[i].expected_CrossEnd   << "  ";
        cout << "obs END: "     << MILL.CrossTimeEnd         << " || ";
        cout << endl;
    }
    return -1;
}



int TEST__DETERMINE_Overleap(void)
{
    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> TEST FOR: DetermineOverleap() " << endl;
    cout << " >>> " << endl;
    cout << endl;

    struct TESTTable
    {
        double T1_CrossStart;
        double T1_CrossEnd;
        double T2_CrossStart;
        double T2_CrossEnd;
        int    ExpectedOverleap;
    };

    int AmountOfTest = 14;
    TESTTable TT[AmountOfTest] =   {
                                         1,  2,  1,  2, overleapExist_YES ,
                                         0,  6,  1,  2, overleapExist_YES ,
                                         0,  1,  1,  2, overleapExist_YES ,
                                         2,  3,  1,  2, overleapExist_YES ,

                                         3,  6,  1,  2, overleapExist_NO  ,
                                       0.1,0.5,  1,  2,  overleapExist_NO ,

                                         1,  2,  1,  2, overleapExist_YES ,
                                         1,  2,  0,  6, overleapExist_YES ,
                                         1,  2,  0,  1, overleapExist_YES ,
                                         1,  2,  2,  3, overleapExist_YES ,

                                         1,  2,  3,  6, overleapExist_NO  ,
                                         1,  2,0.1,0.5,  overleapExist_NO ,


                                         2,  3,  0,  2, overleapExist_YES ,
                                        70, 75, 80, 85, overleapExist_NO,

                                    };




    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL1;
        MILLPEDE MILL2;
        MILL1.CrossTimeStart = TT[i].T1_CrossStart;
        MILL1.CrossTimeEnd   = TT[i].T1_CrossEnd;
        MILL2.CrossTimeStart = TT[i].T2_CrossStart;
        MILL2.CrossTimeEnd   = TT[i].T2_CrossEnd;


        DETERMINE_Overleap(&MILL1, &MILL2);


        if(i+1<10)cout << "TC" <<i+1<<" : ";
        else cout << "TC" <<i+1<<": ";

        if(MILL1.TimesOverleap == TT[i].ExpectedOverleap)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }

        cout << "  ||  ";
        cout << "Start: " <<MILL1.CrossTimeStart <<", ";
        cout << "End: "<<MILL1.CrossTimeEnd <<", ";
        cout << "Start: " <<MILL2.CrossTimeStart <<", ";
        cout << "End: "<<MILL2.CrossTimeEnd <<",   ||  result: ";
        if(MILL1.TimesOverleap) cout << "Overleap_YES";
        else cout << "Overleap_NO";

        cout << endl;
    }

    return -1;
}



int TEST__CheckMillipedeCollision(void)
{
    
    struct TESTTable
    {
        POIN S1P1;
        POIN S1P2;
        double speed1;

        POIN S2P1;
        POIN S2P2;
        double speed2;

        int expected_CollisionState;
        string TestDescriptions;
        };

    int AmountOfTest = 23;
    TESTTable TT[AmountOfTest] =
    {
    //=========================================================
    //        |        |       |  exp        |
    //   P1   |   P2   | speed |  collision  |  Descriptions
    //        |        |       |             |
        2,  0,  0,  0,  1,    // TC 1
        4, -2,  4, -4,  1, collisionYES, "TC101 kolizja - przeciecie po czasie",

       -1,  2, -3,  2,  2,   //  TC 2
        2,  1,  2, -4,  1, collisionYES,"brak",

       -1,  2, -3,  2,  2,   //  TC 3
       -1,  1, -3,  1,  1, collisionNO,"brak",

       -2, -2, -4,  0,  1,   //  TC 4
        1, -1, -4,  -7, 1, collisionYES,"brak",

       -4, 0, -2,  -2,  1,   //  TC 5
        1, -1, -4,  -7, 1, collisionNO,"brak",

       -2, -2, -4,   0, 1,   //  TC 6
        1, -1, -4,  -7, 1, collisionYES,"Zderzenie = punkt zderzenia w miejscu startu drugiej stonogi",

        0,  1, -1,   1, 1,   //  TC 7
        1,  0,  1,  -1, 1, collisionYES,"Zderzenie po 1 sek, proste prostopadle",

        1,  2,  0,   2, 1,   //  TC 8
        2,  1,  2,   0, 1, collisionYES,"Zderzenie po 1 sek, proste prostopadle",

       -2, -2, -4,   0,99,   //  TC 9
        1, -1, -4,  -7, 1, collisionYES,"Zderzenie = punkt zderzenia w miejscu startu drugiej stonogi, pocisk",

    -4.52, 6.28,  4.3,  3.64,  0.01, //TC10
    -4.67, 3.43,-6.49,  1.73,     1, collisionYES,"kolizja - przeciecie po czasie",

    -4.52, 6.28,  4.3,  3.64,  0.5,  //TC11
    -4.67, 3.43,-6.49,  1.73,   200, collisionYES,"kolizja - przeciecie po czasie" ,

    -4.52, 6.28,  4.3,  3.64,    50, //TC12
    -4.67, 3.43,-6.49,  1.73,     1, collisionNO,"przeciecie trajektorii - przeciwny kierunek ???",

    -4.52, 6.28,  4.3,  3.64,  0.01, //TC13
    -6.49, 1.73,-4.67,  3.43,     1, collisionNO,"przeciecie trajektorii - przeciwny kierunek ???",

        0,    2,    0,     0,    10, //TC14
        2,    2,    2,     0,     1, collisionNO,"brak przeciecia Trajektorie rownolegle pionowe",

        3,    3,   -3,     3,   100, //TC15
        3,   -3,   -3,   -3.,    10, collisionNO,"brak przeciecia Trajektorie rownloegle poziome",

//|==============================================================================================================|
//| TRAJEKTORIE POZIOME NAKŁADAJĄCE SIĘ                                                                          |
//|==============================================================================================================|
//|        |        |       |  exp        |                                                                      |
//|   P1   |   P2   | speed |  collision  |  Descriptions                                                        |
//|        |        |       |             |                                                                      |
     -1,1  , -2,1   ,   1   , //TC16
      1,1  ,  2,1   ,   1   , collisionYES,"     KOL - Trajektorie POZIOME sie nakladaja, zderzenie czolowe po czasie 1",

     -2,1  ,  -1,1  ,   1   , //TC17
      1,1  ,   2,1  ,   1   , collisionNO, "BRAK KOL - Trajektorie POZIOME sie nakladaja, poruszanie w lewe ze stala predkoscia",

     -2,1  ,  -1,1  ,   1   , //TC18
      1,1  ,   2,1  ,   2   , collisionYES,"     KOL - Trajektorie POZIOME sie nakladaja, ST2 dogoni ST1",

     -2,1  ,  -1,1  ,   1   , //TC19
      1,1  ,   2,1  ,  0.5  , collisionNO, "BRAK KOL - Trajektorie POZIOME sie nakladaja, ST2 dogoni ST1",

//==============================================================================================================
// TRAJEKTORIE PIONOWE NAKŁADAJĄCE SIĘ
//==============================================================================================================
//        |        |       |  exp        |
//   P1   |   P2   | speed |  collision  |  Descriptions
//        |        |       |             |
    0, 1  ,  0, 2  ,   1   , //TC20 // kolizja w CP (0,0) po czasie 1
    0,-1  ,  0,-2  ,   1   , collisionYES,"     KOL - Trajektorie PIONOWE sie nakladaja, zderzenie czolowe po czasie 1",

    0, 1  ,  0, 2  ,   1   , //TC21 //
    0,-2  ,  0,-1  ,   1   , collisionNO, "BRAK KOL - Trajektorie PIONOWE sie nakladaja, poruszanie w dol ze stala predkoscia",

    0, 1  ,  0, 2  ,   2   , //TC22 // kolizja w CP (0,3) po czasie 2
    0,-2  ,  0,-1  ,   1   , collisionYES, "     KOL - Trajektorie PIONOWE sie nakladaja, kol w CP(0,3) po 2s || ST1 dogoniła ST2",

    0, 1  ,  0, 2  ,  0.3  , //TC23 //
    0,-2  ,  0,-1  ,   1   , collisionNO, "BRAK  KOL - Trajektorie PIONOWE sie nakladaja, ST1 nigdy nie dogini ST@",
//==============================================================================================================
// KOLIZJA SKOŚNA, KOLIZJA GRANICZNA (KONIC Z POCZĄTKIEM)
//==============================================================================================================
//        |        |       |  exp        |
//   P1   |   P2   | speed |  collision  |  Descriptions
//        |        |       |             |
     0,0  ,  -1,1  ,   1   , // TC24
     0,1  ,   1,2  ,   1   , collisionYES,"KOLIZJA SKOŚNA, KOLIZJA GRANICZNA (KONIC Z POCZĄTKIEM)"

    };

    cout << "/n/n";
    cout << " ==========================================================================================================/n";
    cout << " >>>                                                                                                    <<</n";
    cout << " >>> FINAL TESTS                                                                                        <<</n";
    cout << " >>>                                                                                                    <<</n";
    cout << " >>> TEST FOR: CheckMillipedeCollision()                                                                <<</n";
    cout << " >>>                                                                                                    <<</n";
    cout << " ==========================================================================================================/n";
    cout << "/n";

    for(int i=0; i<AmountOfTest; i++)
    {

        MILLPEDE MILL1;
        MILLPEDE MILL2;

        MILL1.P1  = TT[i].S1P1;
        MILL1.P2  = TT[i].S1P2;
        MILL1.speed = TT[i].speed1;

        MILL2.P1  = TT[i].S2P1;
        MILL2.P2  = TT[i].S2P2;
        MILL2.speed = TT[i].speed2;

        #if MODE_DEB == 1
        cout << "/n/n";
        cout << ">>>=========================================================<<</n";
        cout << ">>>  MAIN_TCDEBOUG: " << i+1 <<                            "<<</n";
        cout << ">>>=========================================================<<</n";
        cout << endl;
        cout << "    ST1(P1)(P2) >> "
             << "(" << TT[i].S1P1.x << "," << TT[i].S1P1.y << ")"
             << "(" << TT[i].S1P2.x << "," << TT[i].S1P2.y << ")" << endl;
        cout << "    ST2(P1)(P2) >> "
             << "(" << TT[i].S2P1.x << "," << TT[i].S2P1.y << ")"
             << "(" << TT[i].S2P2.x << "," << TT[i].S2P2.y << ")" << endl;
        cout << "    ------------------------------" << endl;
        cout << endl;
        #endif // MODE_DEB


        int ret=1;
        ret = CHECK_MillipedeCollision(&MILL1, &MILL2);

        if(i+1<10)cout << "TC" <<i+1<<" : ";
        else cout << "TC" <<i+1<<": ";

        if(ret == TT[i].expected_CollisionState)
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }

        cout << "  ||  expected: ";
        if(collisionYES == TT[i].expected_CollisionState) cout << "TAK";
        else cout << "NIE";
        cout << "   observed: ";
        if(collisionYES == ret) cout << "TAK";
        else cout << "NIE";
        cout << "  ||  speed1: " << TT[i].speed1;
        cout << "  ||  Des: " << TT[i].TestDescriptions;
        cout << endl;
    }

    return -1;
}
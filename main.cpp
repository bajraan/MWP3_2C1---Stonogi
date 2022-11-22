#include <iostream>
#include <algorithm>
#include <math.h>
#define MODE_DEB    0
#define MODE_TEST   1

#define TEST_PASS   SetConsoleTextAttribute(hConsole,10);cout<<"PASS";SetConsoleTextAttribute(hConsole,7);
#define TEST_FAIL   SetConsoleTextAttribute(hConsole,12);cout<<"FAIL";SetConsoleTextAttribute(hConsole,7);

#if MODE_TEST == 1
#include <windows.h>
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif // MODE_TEST

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

struct POIN
{
    double x;
    double y;
};

// 0 = Ax - Cy + F
struct LINFAC
{
    double A;
    double F;
    double B;

};

struct MILLPEDE
{
    POIN    P1;
    POIN    P2;
    double  speed;
    double  linea;
    double  lineb;
    LINFAC  lin;
    string  CrossPointExist;
    POIN    CrossPoint;
    int     CPdirection;
    double  CrossTimeStart;
    double  CrossTimeEnd;
    int     TimesOverleap;

};


int CheckMillipedeCollision(MILLPEDE *ST1, MILLPEDE *ST2);
int DetermineIntersectionScenario(MILLPEDE *ST1, MILLPEDE *ST2);
void DetermineLineEquation(MILLPEDE *ST);
void DetermineCrossPoint(MILLPEDE *T1, MILLPEDE *T2);
void DetermineCrossDirection(MILLPEDE *T1);
void DetermineCrossTimes(MILLPEDE *T1);
void DetermineOverleap(MILLPEDE *T1, MILLPEDE *T2);

int TEST_CheckMillipedeCollision(void);
int TEST_DETAILED_CheckMillipedeCollision(void);
int TEST_DetermineLineEquation(void);
int TEST_DetermineCrossPoint(void);
int TEST_DetermineCrossDirection(void);
int TEST_DetermineCrossTimes(void);
int TEST_DetermineOverleap(void);


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
        CheckMillipedeCollision(&ST1, &ST2);
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

int CheckMillipedeCollision(MILLPEDE *ST1, MILLPEDE *ST2)
{
    DetermineLineEquation(ST1); // Determine line eqution for ST!
    DetermineLineEquation(ST2); // Determine line eqution for ST2
    DetermineCrossPoint(ST1,ST2);
    if(ST1->CrossPointExist == "YES")
    {
        DetermineCrossPoint(ST1,ST2);

        DetermineCrossDirection(ST1);
        if(ST1->CPdirection == pos)
        {
            DetermineCrossDirection(ST2);
            if(ST2->CPdirection == pos)
            {
                DetermineCrossTimes(ST1);
                DetermineCrossTimes(ST2);
                DetermineOverleap(ST1,ST2);
                if(overleapExist_YES == ST1->TimesOverleap)
                {
                    return collisionYES;
                }
                else
                {
                    return collisionNO;
                }
            }
            else return collisionNO;
        }
        else return collisionNO;
    }
    else return collisionNO;
}

void DetermineLineEquation(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "DetermineLineEquation"  << endl;
    #endif // MODE_DEB




    if(T->P1.x == T->P2.x)
    {
        T->lin.A =  1;
        T->lin.F = - T->P1.x;
        T->lin.B =  0;
    }
    else
    {
        double Slope;
        double Intercept;
    //
    // SLOPE // a
    //

        if(0 == (max(T->P1.y, T->P2.y) - min(T->P1.y, T->P2.y))) Slope = 0;
        else Slope = (T->P2.y - T->P1.y) / (T->P2.x - T->P1.x);

    //
    // INTERCEPT // b
    //

        if(0 == Slope) Intercept = T->P1.x;
        else Intercept = T->P1.y - Slope * T->P1.x;

        T->lin.A = Slope;
        T->lin.F = Intercept;
        T->lin.B = -1;

    }


    #if MODE_DEB == 1
    cout << endl;
    cout << "A Slope      : " << T->lin.A << endl;
    cout << "B Intercept  : " << T->lin.B << endl;
    cout << "C            : " << T->lin.C << endl;
    cout << "B calculation: " << T->P1.y <<"-"<< T->lin.A <<"*"<< T->P1.x<< endl;
    #endif // MODE_DEB
}

int DetermineIntersectionScenario(MILLPEDE *ST1, MILLPEDE *ST2)
{
    return 0;
}

//
//x = (b1 - b2)/(a2 - a1);
//
//y = (a2 * b1 - b2 * a1) / (a2 - a1);
//
void DetermineCrossPoint(MILLPEDE *T1, MILLPEDE *T2)
{
    #if MODE_DEB == 1
    cout << "DetermineCrossPoint"  << endl;
    #endif // MODE_DEB

    if(!(T1->lin.B)) cout << "ST1 NIE JEST FUNKCJA" << endl;
    if(!(T2->lin.B)) cout << "ST2 NIE JEST FUNKCJA" << endl;

    if(T1->lin.A == T2->lin.A)
    {
        T1->CrossPointExist = "NO";
        T2->CrossPointExist = "NO";
    }
    else
    {
        T1->CrossPointExist = "YES";
        T2->CrossPointExist = "YES";

        T1->CrossPoint.x = (T1->lineb - T2->lineb)
                                /
                    (T2->linea - T1->linea);

        T1->CrossPoint.y = (T2->linea * T1->lineb - T2->lineb * T1->linea )
                                                  /
                                       (T2->linea - T1->linea);
    }
}

// trzeba policzyć który z punktów P1,P2 jest bliżej punktu CP,
// jeżeli punkt P1 jest bliżej CP to kierunek jset pozytywny
// jeżeli punkt P2 jest bliżej CP to kierunek jest negatywny
//
// Examples:
// P1<------P2      CP      // DIRECTION NEGATIVE
// P2------>P1      CP      // DIRECTION POSITIVE
// CP    P1<--------P2      // DIRECTION POSITIVE
// CP    P2-------->P1      // DIRECTION NEGATIVE
void DetermineCrossDirection(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "DetermineCrossDirection" << endl;
    #endif // MODE_DEB

    double cpp1,cpp2;
    cpp1 = sqrt(pow(T->P1.x - T->CrossPoint.x,2) + pow(T->P1.y - T->CrossPoint.y,2));
    cpp2 = sqrt(pow(T->P2.x - T->CrossPoint.x,2) + pow(T->P2.y - T->CrossPoint.y,2));

    if      (cpp1 == cpp2) cout << endl << "FATAL ERROR" << endl;
    else if (cpp1 <  cpp2) T->CPdirection = pos;
    else if (cpp1 >  cpp2) T->CPdirection = neg;
    else                   cout << endl << "FATAL ERROR" << endl;



}

void DetermineCrossTimes(MILLPEDE *T)
{
    #if MODE_DEB == 1
    cout << "DetermineCrossTimes"  << endl;
    #endif // MODE_DEB


    // t=s/v
    double S1;
    double S2;

    #if MODE_DEB == 1

    cout << endl;
    cout << endl;
    cout << "DEBOUG DATA: " << endl;
    cout << "P1:   (" << T->P1.x         <<","<< T->P1.y         << ")" << endl;
    cout << "P2:   (" << T->P2.x         <<","<< T->P2.y         << ")" << endl;
    cout << "CP:   (" << T->CrossPoint.x <<","<< T->CrossPoint.y << ")" << endl;
    cout << "Speed: " << T->speed << endl;
    #endif // MODE_DEB


    S1 =   sqrt
           (
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
        cout << "Fatal Error";
        cout << "Wrong input data!";
    }

    //TODO
    // SPEED == 0
    #if MODE_DEB == 1
    cout << endl;
    cout << "S1: " << S1 << endl;
    cout << "S2: " << S2 << endl;
    if(S1>S2)
    {
    cout << "CrossTimeStart: " << T->CrossTimeStart << endl;
    cout << "Calc: " << S2<<"/"<<T->speed << endl;
    cout << "CrossTimeEnd: " << T->CrossTimeEnd << endl;
    cout << "Calc: " << S1<<"/"<< T->speed;
    }
    if(S1<S2)
    {
    cout << "CrossTimeStart: " << T->CrossTimeStart << endl;
    cout << "Calc: " << S1<<"/"<<T->speed << endl;
    cout << "CrossTimeEnd: " << T->CrossTimeEnd << endl;
    cout << "Calc: " << S2<<"/"<< T->speed;
    }
    cout << endl;
    #endif // MODE_DEB

}

void DetermineOverleap(MILLPEDE *T1, MILLPEDE *T2)
{
    #if MODE_DEB == 1
    cout << "DetermineOverleap"  << endl;
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




}

//
//===============================================================
// FUNKCJE TESTUJ¥CE
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
    TEST_DetermineLineEquation();
    TEST_DetermineCrossPoint();
    TEST_DetermineCrossDirection();
    TEST_DetermineCrossTimes();
    TEST_DetermineOverleap();
    TEST_CheckMillipedeCollision();
    #endif // MODE_TEST
}
/*
int TEST_DetermineLineEquation( void )
{


    int AmountOfTest = 5;
    struct TESTTable
{
    double P1x;
    double P1y;
    double P2x;
    double P2y;
    double expected_a;
    double expected_b;
};

    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> Determine line equation TESTS" << endl;
    cout << " >>> " << endl;
    cout << endl;

    TESTTable TT[AmountOfTest] =   {-1,-2, 1, 2, 2, 0,
                                     3, 4, 3,-2, 0, 3,
                                    -4,-2,-1,-4, (double)-2/3, (double)-14/3,
                                    -1, 2, 9, 0, (double)-1/5, (double)9/5,
                                     1, 1, 2, 2, 1, 0                           };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL = {0,0,0,0,0,0,0};
        MILL.P1.x = TT[i].P1x;
        MILL.P1.y = TT[i].P1y;
        MILL.P2.x = TT[i].P2x;
        MILL.P2.y = TT[i].P2y;

        DetermineLineEquation(&MILL);

        cout << "TC" << i+1 << ": ";
        if((fabs(MILL.linea - TT[i].expected_a) <= 1e-10) && (fabs(MILL.lineb  - TT[i].expected_b) <= 1e-10))
        {
            TEST_PASS;
        }
        else
        {
            TEST_FAIL;
        }


        cout << "  ||  ";
        cout << "P1("<<TT[i].P1x<<","<<TT[i].P1y<<")";
        cout << "P2("<<TT[i].P2x<<","<<TT[i].P2y<<")";
        cout << "   ";
        cout << "a: " << MILL.linea << " EXP: " << TT[i].expected_a;
        cout << "   ";
        cout << "b: " << MILL.lineb << " EXP: " << TT[i].expected_b;
        cout << endl;
    }
    return 0;
}
*/


int TEST_DetermineLineEquation( void )
{

    int AmountOfTest = 7;
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

    TESTTable TT[AmountOfTest] =

                                {
                                // 0 = Ax + B - Cy
                                // Non Function Cases:
                                10, 0,10,10,            1,           -10,  0,
                                 0, 4, 0, 2,            1,             0,  0,
                                 3, 4, 3,-2,            1,            -3,  0,
                                // Function Cases:
                                -1,-2, 1, 2,            2,             0, -1,
                                -4,-2,-1,-4, (double)-2/3, (double)-14/3, -1,
                                -1, 2, 9, 0, (double)-1/5, (double)  9/5, -1,
                                 1, 1, 2, 2,            1,             0, -1
                                };


    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL = {0,0,0,0,0,0,0};
        MILL.P1 = TT[i].P1;
        MILL.P2 = TT[i].P2;

        DetermineLineEquation(&MILL);

        cout << "TC" << i+1 << ": ";
        if
        (
             (fabs(MILL.lin.A - TT[i].expected.A) <= 1e-10)
             &&
             (fabs(MILL.lin.F - TT[i].expected.F) <= 1e-10)
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
        cout << "Ax: " << MILL.lin.A << " EXP: " << TT[i].expected.A;
        cout << "    ";
        cout << "F:  " << MILL.lin.F << " EXP: " << TT[i].expected.F;
        cout << "    ";
        cout << "By:  " << MILL.lin.B << " EXP: " << TT[i].expected.B;
        cout << endl;
    }
    return 0;
}

int TEST_DetermineCrossPoint(void)
{

    // 0 = ST2A * X + ST2B + Y

    struct TESTTable

{
    POIN    ST1P1;
    POIN    ST1P2;
    POIN    ST2P1;
    POIN    ST2P2;
    string  Expected_CrossPointExist;
    POIN   Expected_CP;

};


    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> Determine cross point TESTS" << endl;
    cout << " >>> " << endl;
    cout << endl;

    int AmountOfTest = 6;
    TESTTable TT[AmountOfTest] =
    {
        /*
        ███ ████ ███ ████
        █   █  █  █  █  █
        ███ ████  █  ████ █  █  █
        */
        //---------------+---------------+------+------+
        //ST1             |ST2            |CP    |CP    |
        //P1      |P2     |P1     |P2     |EXIST |Expect|
        //        |       |       |       |      |      |
            0,   2,  0,  4,  2,  0,  4,  0,"YES" ,  0, 0,
           -2,   0, -4,  0,  0, -2,  0, -4,"YES" ,  0, 0,
           -1,   1, -2,  2,  1,  1,  2,  2,"YES" ,  0, 0,
           -1,   2, -2,  3,  1,  2,  2,  3,"YES" ,  0, 1,
            0,   1, -1,  1,  1,  0,  1, -1,"YES" ,  1, 1,
            0,   1,  1,  1,  0,  2,  1,  2,"NO"  ,  0, 0
    };



    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL1;
        MILLPEDE MILL2;

        DetermineCrossPoint(&MILL1, &MILL2);

        cout << "TC" << i+1 << ": ";

        if
        (        (MILL1.CrossPointExist == TT[i].Expected_CrossPointExist)
                &&
             (fabs(MILL1.CrossPoint.x    - TT[i].Expected_CP.x) <=1e-10)
                &&
             (fabs(MILL1.CrossPoint.y    - TT[i].Expected_CP.y) <=1e-10)
        )
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
        cout << "x:      "<<MILL1.CrossPoint.x << TT[i].Expected_CP.x << endl;
        cout << "logicx: "<<(fabs(MILL1.CrossPoint.x    - TT[i].Expected_CP.x) <=1e-10) << endl;
        cout << "fabs:   "<<fabs(MILL1.CrossPoint.y    - TT[i].Expected_CP.y) << endl;
        cout << "y:      "<<MILL1.CrossPoint.y << TT[i].Expected_CP.y << endl;
        cout << "logicx: "<< (fabs(MILL1.CrossPoint.x    - TT[i].Expected_CP.x) <=1e-10) << endl;
        #endif // MODE_DEB


    }
    return 0;
}

int TEST_DetermineCrossDirection(void)
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

    int AmountOfTest = 9;
    TESTTable TT[AmountOfTest] =   {    //-1,-2, 1, 2,          -2,             0,
                                        // 3, 4, 3,-2,           0,             0,
                                        //-4,-2,-1,-4,(double) 2/3, (double)-14/3,
                                          -1, 2, 9, 0,          10,           -0.2, neg,
                                          -1, 2, 9, 0,          20,          9/5-4, neg,
                                          -1, 2, 9, 0,         -20,          9/5+4, pos,
                                           1, 1, 2, 2,           0,              0, pos,
                                           1, 1, 2, 2,        -111,           -111, pos,
                                           1, 1, 2, 2,      -60000,         -60000, pos,
                                           1, 1, 2, 2,           3,              3, neg,
                                          -1,-2, 1, 2,           2,              4, neg,
                                          -1,-2, 1, 2,          -2,             -4, pos     };




    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL;

        MILL.P1.x          = TT[i].P1.x;
        MILL.P1.y          = TT[i].P1.y;
        MILL.P2.x          = TT[i].P2.x;
        MILL.P2.y          = TT[i].P2.y;
        MILL.CrossPoint.x  = TT[i].CP.x;
        MILL.CrossPoint.y  = TT[i].CP.y;


        DetermineCrossDirection(&MILL);


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



}
int TEST_DetermineCrossTimes(void)
{
    struct TESTTable
{
    POIN P1;
    POIN P2;
    int direction;
    double speed;
    POIN CP;
    double expected_CrossStart;
    double expected_CrossEnd;
};

    int AmountOfTest = 6;
    TESTTable TT[AmountOfTest] =   {     1, 0, 0, 0,pos  ,              1,        2,  0,  1,  2,
                                        -1,-2, 1, 2,pos  ,(double)sqrt(5),        2,  4,  1,  3,
                                         4, 3, 4,-1,pos  ,              2,        4,  5,  1,  3,
                                         3, 2, 1, 2,pos  ,              1,        4,  2,  1,  3,
                                        -2, 2,-6,-2,pos  ,(double)sqrt(8),        0,  4,  1,  3,
                                       100, 0,200,0,pos  ,              1,        0,  0,100,200
                                         };


    cout << endl;
    cout << " >>> " << endl;
    cout << " >>> TEST FOR: DetermineCrossTimes() " << endl;
    cout << " >>> " << endl;
    cout << endl;


    for(int i=0; i<AmountOfTest; i++)
    {

        MILLPEDE MILL;
        MILL.P1         = TT[i].P1;
        MILL.P2         = TT[i].P2;
        MILL.CrossPoint = TT[i].CP;
        MILL.speed      = TT[i].speed;

        DetermineCrossTimes(&MILL);

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

}

int TEST_DetermineOverleap(void)
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
                                        70, 75, 80, 85, overleapExist_NO
                                    };




    for(int i=0; i<AmountOfTest; i++)
    {
        MILLPEDE MILL1;
        MILLPEDE MILL2;
        MILL1.CrossTimeStart = TT[i].T1_CrossStart;
        MILL1.CrossTimeEnd   = TT[i].T1_CrossEnd;
        MILL2.CrossTimeStart = TT[i].T2_CrossStart;
        MILL2.CrossTimeEnd   = TT[i].T2_CrossEnd;


        DetermineOverleap(&MILL1, &MILL2);


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

}

int TEST_CheckMillipedeCollision(void)
{
    int AmountOfTest = 19;
    struct TESTTable
{
    double p11x;
    double p11y;
    double p12x;
    double p12y;
    double speed1;
    double p21x;
    double p21y;
    double p22x;
    double p22y;
    double speed2;
    int expected_CollisionState;
    string TestDescriptions;
};
    TESTTable TT[AmountOfTest] =
    {
        2,  0,  0,  0,  1,    // TC 1
        4, -2,  4, -4,  1, collisionYES, "kolizja - przeciecie po czasie",

       -1,  2, -3,  2,  2,   //  TC 2
        2,  1,  2, -4,  1, collisionYES,"brak",

       -1,  2, -3,  2,  2,   //  TC 3
       -1,  1, -3,  1,  1, collisionNO,"brak",

       -2, -2, -4,  0,  1,   //  TC 4
        1, -1, -4,  -7, 1, collisionYES,"brak",

       -4, 0, -2,  -2,  1,   //  TC 5
        1, -1, -4,  -7, 1, collisionNO,"brak",

    -4.52, 6.28,  4.3,  3.64,  0.01,   //  TC 6
    -4.67, 3.43,-6.49,  1.73,     1, collisionYES,"kolizja - przeciecie po czasie",

    -4.52, 6.28,  4.3,  3.64,  0.5,   //  TC7
    -4.67, 3.43,-6.49,  1.73,   200, collisionYES,"kolizja - przeciecie po czasie" ,

    -4.52, 6.28,  4.3,  3.64,    50,   //  TC8
    -4.67, 3.43,-6.49,  1.73,     1, collisionNO,"przeciecie trajektorii - przeciwny kierunek ???",

    -4.52, 6.28,  4.3,  3.64,  0.01,   //  TC9
    -6.49, 1.73,-4.67,  3.43,     1, collisionNO,"przeciecie trajektorii - przeciwny kierunek ???",

        0,    2,    0,     0,    10,   //  TC10
        2,    2,    2,     0,     1, collisionNO,"brak przeciecia Trajektorie rownolegle pionowe",

        3,    3,   -3,     3,   100,  //  TC11
        3,   -3,   -3,   -3.,    10, collisionNO,"brak przeciecia Trajektorie rownloegle poziome",

//==============================================================================================================
// TRAJEKTORIE POZIOME NAKŁADAJĄCE SIĘ
//==============================================================================================================
       -1,    1,   -2,     1,     1, //  TC12
        1,    1,    2,     1,     1, collisionYES,"     KOL - Trajektorie POZIOME sie nakladaja, zderzenie czolowe po czasie 1",

       -2,    1,   -1,     1,     1, //  TC13
        1,    1,    2,     1,     1, collisionNO, "BRAK KOL - Trajektorie POZIOME sie nakladaja, poruszanie w lewe ze stala predkoscia",

       -2,    1,   -1,     1,     1, //  TC14
        1,    1,    2,     1,     2, collisionYES,"     KOL - Trajektorie POZIOME sie nakladaja, ST2 dogoni ST1",

       -2,    1,   -1,     1,     1, //  TC15
        1,    1,    2,     1,   0.5, collisionNO, "BRAK KOL - Trajektorie POZIOME sie nakladaja, ST2 dogoni ST1",

//==============================================================================================================
// TRAJEKTORIE PIONOWE NAKŁADAJĄCE SIĘ
//==============================================================================================================
        0,    1,    0,     2,     1, //  TC16 // kolizja w CP (0,0) po czasie 1
        0,   -1,    0,    -2,     1, collisionYES,"     KOL - Trajektorie PIONOWE sie nakladaja, zderzenie czolowe po czasie 1",

        0,    1,    0,     2,     1, //  TC17
        0,   -2,    0,    -1,     1, collisionNO, "BRAK KOL - Trajektorie PIONOWE sie nakladaja, poruszanie w dol ze stala predkoscia",

        0,    1,    0,     2,     2, //  TC18 // kolizja w CP (0,3) po czasie 2
        0,   -2,    0,    -1,     1, collisionYES, "     KOL - Trajektorie PIONOWE sie nakladaja, kol w CP(0,3) po 2s || ST1 dogoniła ST2",

        0,    1,    0,     2,   0.3, //  TC19 //
        0,   -2,    0,    -1,     1, collisionNO, "BRAK  KOL - Trajektorie PIONOWE sie nakladaja, ST1 nigdy nie dogini ST@",


    };

    cout << endl << endl;
    cout << " ====================================================================" << endl;
    cout << " >>>                                                              <<<" << endl;
    cout << " >>> FINAL TESTS                                                  <<<" << endl;
    cout << " >>>                                                              <<<" << endl;
    cout << " >>> TEST FOR: CheckMillipedeCollision()                          <<<" << endl;
    cout << " >>>                                                              <<<" << endl;
    cout << " ====================================================================" << endl;
    cout << endl;

    for(int i=0; i<AmountOfTest; i++)
    {

        MILLPEDE MILL1;
        MILLPEDE MILL2;

        MILL1.P1.x  = TT[i].p11x;
        MILL1.P1.y  = TT[i].p11y;
        MILL1.P2.x  = TT[i].p12x;
        MILL1.P2.y  = TT[i].p12y;
        MILL1.speed = TT[i].speed1;
        MILL2.P1.x  = TT[i].p21x;
        MILL2.P1.y  = TT[i].p21y;
        MILL2.P2.x  = TT[i].p22x;
        MILL2.P2.y  = TT[i].p22y;
        MILL2.speed = TT[i].speed1;

        int ret=1;
        ret = CheckMillipedeCollision(&MILL1, &MILL2);

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
}

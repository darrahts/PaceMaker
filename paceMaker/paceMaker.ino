#include <stdio.h>

#define PACE_SIGNAL_PIN 2
#define ECG_IN_PIN A1


enum State
{
    A = 0, // initial state
    P, //
    Q, //
    R, //
    Rp, // peak
    S, //
    T,  //
    E,  // error
};


//******************************************************* VARIABLES

bool timerReady = true;

long t1 = millis();
long t2 = millis();
long initTime = millis();

long errorTime = millis();
long lastPace = millis();

long beatTime = millis();

int reading = 0;
int val = 0;
enum State state = A;

long duration = 0;

bool paced = false;

bool triggered = false;
//bool initialized = false;

int beat = 0;
int bpm = 0;

int pC = 0;
int qC = 0;
int rC = 0; 
int rpC = 0;
int sC = 0;
int tC = 0;

int timesp[] = {0,0,0,0,0,0,0,0,0,0};
int timesq[] = {0,0,0,0,0,0,0,0,0,0};
int timesr[] = {0,0,0,0,0,0,0,0,0,0};
int timesrp[] = {0,0,0,0,0,0,0,0,0,0};
int timess[] = {0,0,0,0,0,0,0,0,0,0};
int timest[] = {0,0,0,0,0,0,0,0,0,0};

int p = 0;
int q = 0;
int r = 0;
int rp = 0;
int s = 0;
int t = 0;


//******************************************************* PACE
void Pace()
{
    paced = true;
    if(millis() - lastPace > 879)
    {
        digitalWrite(PACE_SIGNAL_PIN, HIGH);
        errorTime = millis();
        while(reading > 160 || reading < 120)
        {
            Poll();
            if(millis() - errorTime > 20)
            {
                state = E;
                //Serial.print("Setting state: ");
                //Serial.println(int(state));
                paced = false;
                break;
            }
        }
        //Serial.println(millis() - errorTime);
        digitalWrite(PACE_SIGNAL_PIN, LOW);
        lastPace = millis();
    }
    return;
}

//******************************************************* POLL
void Poll()
{
    reading = int(analogRead(ECG_IN_PIN));
    return;
}

//******************************************************* UPDATE STATE VARIABLES

void UpdateStateVariables(int &arrayCounter,int &stateCounter, int timesArray[])
{
//    Serial.print("state: ");
//    Serial.print(int(state));
//    Serial.print("     count: ");
//    Serial.println(stateCounter);
    timesArray[arrayCounter] = stateCounter;
    stateCounter = 0;
    arrayCounter++;
//    Serial.print("arrayCounter: ");
//    Serial.println(arrayCounter);
    if(arrayCounter == 10)
    {
//        Serial.print("state: ");
//        Serial.print(int(state));
//        Serial.print("     count: ");
//        for(int j = 0; j < 10; j++)
//        {
//            Serial.print(timesArray[j]);
//            Serial.print(", ");
//        }
//        Serial.println();
        arrayCounter = 0;
    }
}

void PrintHeader()
{
    if(state == P)
    {
        Serial.println("******");
    }
    Serial.print("state: ");
    Serial.print(int(state));
}

void PrintBody()
{
    Serial.print("     duration: ");
    Serial.print(duration);
    if(state == Rp || state == S)
    {
        Serial.print(" ");
    }
    Serial.print("     next state: ");
    Serial.print(int(state));    
    Serial.println();
}

//******************************************************* SetState
void SetState(int nextState)
{
    //PrintHeader();
    state = State(nextState);
    if(!timerReady)
    {
        t2 = millis();
        duration = (t2-t1);
        timerReady = true;
        //PrintBody();
    }
    if(timerReady)
    {
        t1 = millis();
        timerReady = false;
    }
    return;
}

//******************************************************* STEP
void Step()
{
    
    if(state == A)
    {
        //Serial.println("A");
        if(reading > 150)
        {
            SetState(state+1);
        }
        return;
    }
    
    if(state == P)
    {
        //Serial.println("P");
        pC++;
        if(reading > 200 && reading < 225)
        {
            UpdateStateVariables(p, pC, timesp);
            SetState(state+1);
        }
        return;
    }
    if(state == Q)
    {
        //Serial.println("Q");
        qC++;
        if(millis() - t1 > 140 && !triggered)
        {
            Pace();
            triggered = true;
        }
        else if(qC > 29 && !triggered)
        {
            Pace();
            triggered = true;
        }
        if(triggered || (reading < 140 && reading > 130))
        {
            UpdateStateVariables(q, qC, timesq);
            if(paced)
            {
                SetState(state+1);
            }
        }
        return;
    }
    if(state == R)
    {
        //Serial.println("R");
        if(triggered)
        {
            triggered = false;
        }
        rC++;
        if(reading > 550)
        {
            UpdateStateVariables(r, rC, timesr);
            SetState(state+1);
            beat++;
            if(beat == 12)
            {
                bpm = int(60 / double((millis() - beatTime) / beat) * 1000);
                //Serial.print("BPM: ");
                //Serial.println(bpm);
                beat = 0;
                beatTime = millis();      
            }
        }
        return;
    }

    if(state == Rp)
    {
       //Serial.println("Rp");
        rpC++;
        if(reading < 150)
        {
            UpdateStateVariables(rp, rpC, timesrp);
            SetState(state+1);
        }
        return;
    }
    
    if(state == S)
    {
        //Serial.println("S");
        sC++;
        if(reading > 195)
        {
            UpdateStateVariables(s, sC, timess);
            SetState(state+1);
        }
        return;
    }
    
    if(state == T)
    {
        //Serial.println("T");
        tC++;
        if(reading < 195 && reading > 188)
        {
            UpdateStateVariables(t, tC, timest);
            SetState(1);
        }

        return;
    }
    if(state == E)
    {
        Serial.println("ERROR");
        paced = true;
        SetState(0);
    }
    else
    {
        Serial.println("State: ");
        Serial.println(state);
    }
}

//*******************************************************
//******************************************************* SETUP
//*******************************************************
void setup() 
{
    pinMode(ECG_IN_PIN, INPUT);
    pinMode(PACE_SIGNAL_PIN, OUTPUT);
    digitalWrite(PACE_SIGNAL_PIN, LOW);
    Serial.begin(9600);
    delay(3000);
    Serial.println("initialized.");
    beatTime = millis();
}

//*******************************************************
//******************************************************* 
//******************************************************* MAIN
long z1 = micros();
void loop() 
{
     //z1 = micros();
     Poll();
     Step();
     delay(5);
     //Serial.println(micros() - z1);
}


void PrintVal()
{
    Serial.print(reading);
    Serial.print(", ");
}


//******************************************************* TEST
void Test()
{
   while(true)
    {
        delay(8);
        int reading = int(analogRead(A1));
        if(reading > 115 && val != reading)
        {
            if(!timerReady)
            {
                t1 = millis();
                timerReady = true;
            }
            Serial.print(analogRead(A1));
            Serial.print(",");
            val = reading; 
        }
        if(reading < 115)
        {
          //Serial.print("reading is: ");
          //Serial.println(reading);
            if(timerReady)
            {
                Serial.println();
                Serial.println();
                t2 = millis();
                Serial.println(t2-t1);
                timerReady = false;
                Serial.println();
                Serial.println();
            }
        }
    }
}










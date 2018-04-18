

#define BTN 52

#define PACE_SIGNAL_PIN 2

double ecgVals[] = {.025, .02, .022, .018, .018, .02, .01, .1, .19, .1, -.05, -.07, -.14, .6, 1.2, .6, -.2, -.04, .01, .02, .04, .03, .08, .15, .19, .2, .21, .2, .19, .15, .08, .06, .05, .04, .03, .025};
const int len = sizeof(ecgVals)/sizeof(double);
double newEcgVals[len];

double ecgValsSlow[] = {.025, .02, .022, .018, .018, .02, .01, .1, .19, .16, .14, .12, .1, 0, -.01, -.02, -.03, -.04, -.04, -.04, -.05, -.06, -.07, -.07, -.08, -.14, .6, 1.2, .6, -.2, -.04, .01, .02, .04, .03, .08, .15, .19, .2, .21, .2, .19, .15, .08, .06, .05, .04, .03, .025};
const int len2 = sizeof(ecgValsSlow)/sizeof(double);
double newEcgValsSlow[len2];

int mode = 1;
int BTNState = 1;
volatile int j = 0;

int value = 0;

volatile bool paced = false;
//volatile bool ignore = false;

//volatile int k = 0;

long t1 = millis();

void Pace()
{
    if(j > 11 && j < 17)
    {
       // k = j;
        paced = true;
        j = 24;
        return;
    }
    else
    {
        //ignore = true;
    }
    return;
}

//*********************************************************************** MAP

int MapF(double x, double inMin, double inMax, double outMin, double outMax)
{
    return int((x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
}

//***********************************************************************
//*********************************************************************** SETUP
//***********************************************************************
void setup() 
{
    SerialUSB.begin(9600);
    //Serial.begin(9600);
    while(!SerialUSB) ;
    for(int i=0; i < len; i++)
    {
        newEcgVals[i] = MapF(ecgVals[i], -.2, 1.2, 50, 4095);
        SerialUSB.print(newEcgVals[i]);
        SerialUSB.print(", ");
    }
    SerialUSB.println();

    for(int i=0; i < len2; i++)
    {
        newEcgValsSlow[i] = MapF(ecgValsSlow[i], -.2, 1.2, 50, 4095);
        SerialUSB.print(newEcgValsSlow[i]);
        SerialUSB.print(", ");
    }
    SerialUSB.println();
    SerialUSB.println(mode);

    analogWriteResolution(12);
    pinMode(DAC1, OUTPUT);
    pinMode(BTN, INPUT_PULLUP);
    analogWrite(DAC1, 0);
    delay(3000);

    attachInterrupt(digitalPinToInterrupt(PACE_SIGNAL_PIN), Pace, RISING);
    delay(5000);

}

void loop() 
{
//    if(paced)
//    {
//        SerialUSB.print("PACED at: ");
//        SerialUSB.println(k);
//        paced = false;
//    }
//    if(ignore)
//    {
//        SerialUSB.print("IGNORED at: ");
//        SerialUSB.println(k);
//        ignore = false;
//    }
    BTNState = digitalRead(BTN);
    if(mode != BTNState)
    {
        mode = BTNState;
        SerialUSB.print("mode: ");
        SerialUSB.println(mode);
    }
    if(BTNState == 1)
    {
        while(j < len)
        {
            analogWrite(DAC1,newEcgVals[j]);
            j++;
            delay(26);
            //SerialUSB.println(newEcgVals[j++]);
        }
        j = 0;
    }
    else
    {
        while(j < len2)
        {
            analogWrite(DAC1,newEcgValsSlow[j]);
            j++;
            delay(26);
            //SerialUSB.println(newEcgValsSlow[j++]);
        }
        j = 0;
    }
}


void Test()

{
    analogWrite(DAC1, value );  
    value++;
    delay(5);
    if(value >= 4095)
    {
        value = 0;
        delay(5000); 
    }
}


void Test2()
{
    analogWrite(DAC1, 0);
    delay(25);
    int k = 0;
    for(int m = 0; m < 1; m++)
    {
        while(k < len)
        {
            analogWrite(DAC1,newEcgVals[k]);
            k++;
            delay(26);
            //Serial.println(newEcgVals[k++]);
            //SerialUSB.println(newEcgVals[k++]);
        }
        k = 0;
    }
    analogWrite(DAC1, 0);
    delay(10000);
    //Serial.println();
    //SerialUSB.println("waiting");
}












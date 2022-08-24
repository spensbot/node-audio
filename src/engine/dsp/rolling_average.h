

/*
  ==============================================================================
    RollingAverage.h
    Created: 31 Oct 2021 8:37:43pm
    Author:  Spenser Saling
  ==============================================================================
*/

#pragma once

#include <cmath>

class RollingAverage {
public:
    /**
     1.0 < sustainSamples < inf
     inf never incorporates new samples
     0 takes the current sample as the average.
     */
    void setSustainSamples(float sustainSamples){
        if (sustainSamples < 1.0) {
            newWeight = 1.0f;
            oldWeight = 0.0f;
        } else {
            newWeight = 1.0f / sustainSamples;
            oldWeight = 1.0f - newWeight;
        }
    }
    
    void setSustainSeconds(float sampleRate, float sustainSeconds){
        setSustainSamples(sampleRate * sustainSeconds);
    }
    
    void push(float sample){
        average = newWeight * sample + oldWeight * average;
    }
    
    float get(){
        return average;
    }
    
    void reset(){
        average = 0.0f;
    }
    
private:
    float average = 0.0f;
    float newWeight = 1.0f;
    float oldWeight = 0.0f;
};

/** Similar to rolling average, but the average reacts differently to values above/below the current average. */
class RollingAverageBiased {

public:
    void setSustainSamples(float riseSamples, float fallSamples) {
        if (riseSamples < 1.0) {
            weightUpNew = 1.0;
            weightUpOld = 0.0;
        } else {
            weightUpNew = 1.0f/riseSamples;
            weightUpOld = 1.0f - weightUpNew;
        }
        if (fallSamples < 1.0) {
            weightDownNew = 1.0;
            weightDownOld = 0.0;
        } else {
            weightDownNew = 1.0f/fallSamples;
            weightDownOld = 1.0f - weightDownNew;
        }
    }

    void setSustainSeconds(float sampleRate, float riseSeconds, float fallSeconds){
        setSustainSamples(riseSeconds * sampleRate, fallSeconds * sampleRate);
    }

    void push(float sample) {
        if(sample > average){
            // Average moves up
            average = weightUpNew * sample + weightUpOld * average;
        } else {
            // Average moves down
            average = weightDownNew * sample + weightDownOld * average;
        }
    }

    float get() {
        return average;
    }

    void reset(){
        average = 0.0f;
    }

private:
    float average = 0.0f;
    float weightUpNew = 1.0f;
    float weightUpOld = 1.0f;
    float weightDownNew = 0.0f;
    float weightDownOld = 0.0f;
};


/**
 A fast, not entirely accurate way of keeping a running tally of RMS that forgets old values.
 Since RMS is the sqrt of the average of the squared samples
 We just need to maintain a rolling average of the samples squares
 Then return the square root of that average whenever RMS is needed.
 */
class RollingRMS {
public:
    
    void setSustainSeconds(double sampleRate, float sustainSeconds){
        meanSquare.setSustainSeconds(sampleRate, sustainSeconds);
    }
    
    void push(float sample){
        meanSquare.push(sample * sample);
    }
    
    float get(){
        return sqrt( (double)meanSquare.get() );
    }
    
    void reset(){
        meanSquare.reset();
    }
    
private:
    RollingAverage meanSquare;
};


class RollingRMSBiased {
public:
    void setSustainSeconds(double sampleRate, float riseSeconds, float fallSeconds){
        meanSquare.setSustainSeconds(sampleRate, riseSeconds, fallSeconds);
    }

    void push(float sample) {
        meanSquare.push(sample * sample);
    }

    float get() {
        return sqrt( (double) meanSquare.get() );
    }

    void reset(){
        meanSquare.reset();
    }

private:
    RollingAverageBiased meanSquare;
};
        
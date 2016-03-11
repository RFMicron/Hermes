/*
 * Copyright (c) 2016 RFMicron, Inc. (www.rfmicron.com)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to
 * do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <QtCore>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include "gpio.h"

using namespace exploringBB;
using namespace std;

int readAnalog(int number, int num_samples)
{
    stringstream ss;
    ss << "/sys/bus/iio/devices/iio:device0/in_voltage" << number << "_raw";

    fstream fs;
    fs.open(ss.str().c_str(), fstream::in);

    int total = 0;
    for (int i = 0; i < num_samples; i++)
    {
        fs >> number;
        total += number;
    }
    total /= num_samples;

    fs.close();
    return total;
}

class LedThread : public QThread
{
    Q_OBJECT

private:
    bool running;
    bool pass;

public:
    LedThread(void)
    {
        running = true;
        pass = false;
    }

    void run() Q_DECL_OVERRIDE
    {
        GPIO hbLED(60);
        GPIO userLED(46);

        hbLED.setDirection(OUTPUT);
        userLED.setDirection(OUTPUT);
        userLED.setValue(LOW);

        while(running)
        {
            hbLED.setValue(HIGH);
            usleep(250000);
            hbLED.setValue(LOW);
            usleep(250000);
        }

        if (pass) userLED.setValue(HIGH);

        // It keeps running forever so the LEDs can keep the test result
       while (true) { usleep(1000); }
    }

public slots:
    void handleResult(bool result) { pass = result; running = false; }

};

class TestTask : public QObject
{
    Q_OBJECT

private:
    int vdd3V3B;
    bool testVDD_3V3B()
    {
        vdd3V3B = readAnalog(7, 20);

        // ADC reference voltage = 1.8V
        // 12 Bit ADC -> 4096
        // Voltage = ADC * 1.8 / 4096
        // Voltage must be between 1.6V and 1.75V
        return (vdd3V3B >= 3641 && vdd3V3B <= 3982);
    }

    int bbbCurrent;
    bool testBBB_CurrentConsumption(int lowerLimit_mA, int upperLimit_mA)
    {
        bbbCurrent = readAnalog(3, 20);

        // Current sense resistor = 0.01 ohms
        // Current amplifier gain = 25
        // ADC reference voltage = 1.8V
        // 12 Bit ADC -> 4096
        // Current = ADC * 1.8 / (4096 * 25 * 0.01)
        // ADC = Current(A) * 4096 * 25 * 0.01 / 1.8
        int ll_adc = lowerLimit_mA * 0.001 * 4096.0 * 25.0 * 0.01 / 1.8;
        int ul_adc = upperLimit_mA * 0.001 * 4096.0 * 25.0 * 0.01 / 1.8;

        return (bbbCurrent >= ll_adc &&  bbbCurrent <= ul_adc);
    }

    int lcdOffVoltage;
    bool testLcdOff_Voltage()
    {
        lcdOffVoltage = readAnalog(6, 20);

        // Voltage Divider 1 K Ohm and 49.9 Ohm
        // ADC reference voltage = 1.8V
        // 12 Bit ADC -> 4096
        // Voltage = ADC * 1.8 * 1049.9 / (4096 * 49.9)
        // ADC = Voltage(V) * 4096 * 49.9 / (1.8 * 1049.9)
        int ll_adc = 4.5 * 4096.0 * 49.9 / (1.8 * 1049.9);
        int ul_adc = 5.1 * 4096.0 * 49.9 / (1.8 * 1049.9);

        return (lcdOffVoltage >= ll_adc &&  lcdOffVoltage <= ul_adc);
    }

    int lcdOnVoltage;
    bool testLcdOn_Voltage()
    {
        lcdOnVoltage = readAnalog(6, 20);

        // Voltage Divider 1 K Ohm and 49.9 Ohm
        // ADC reference voltage = 1.8V
        // 12 Bit ADC -> 4096
        // Voltage = ((ADC * 1.8 / 4096) * 1049.9 -0.3 * 1000)/ (49.9)
        // ADC = (Voltage(V) * 49.9 + 0.3 * 1000) * 4096 / (1.8 * 1049.9)
        int ll_adc = (8.3 *  49.9 + 300.0) * 4096.0 / (1.8 * 1049.9);
        int ul_adc = (8.9 *  49.9 + 300.0) * 4096.0 / (1.8 * 1049.9) ;

        return (lcdOnVoltage >= ll_adc &&  lcdOnVoltage <= ul_adc);
    }

    int radonCurrent[2];
    bool testRadon_CurrentConsumption(int lowerLimit_mA, int upperLimit_mA, int index)
    {
        radonCurrent[index] = readAnalog(2, 20);

        // Current sense resistor = 0.01 ohms
        // Current amplifier gain = 25
        // ADC reference voltage = 1.8V
        // 12 Bit ADC -> 4096
        // Current = ADC * 1.8 / (4096 * 25 * 0.01)
        // ADC = Current(A) * 4096 * 25 * 0.01 / 1.8
        int ll_adc = lowerLimit_mA * 0.001 * 4096.0 * 25.0 * 0.01 / 1.8;
        int ul_adc = upperLimit_mA * 0.001 * 4096.0 * 25.0 * 0.01 / 1.8;

        return (radonCurrent[index] >= ll_adc &&  radonCurrent[index] <= ul_adc);
    }


    bool program_EEPROM()
    {
        char mem[] = {
            0xaa, 0x55, 0x33, 0xee,  // Header (4)
            'A', '1',                // EEPROM Revision (2)
            'R','F','M','I','C','R','O','N','-','H','E','R','M','E','S', //Board Name
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Rest of the Name (32)
            '0', '0', 'A', '0',      // Version (4)
            'R','F','M','i','c','r','o','n',',',' ','I','n','c','.', 0, 0, // Manufacturer (16)
            'R','F','M','I','C','R','O','N','-','H','E','R','M','E','S', 0, // Part Number (16)
            '0', '0', // Number of Pins (2)
            '0','5','1','6','W','T','H','R','0','0','0','1', // Serial Number (12)
            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, // Pin usage
            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, // Pin usage
            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, // Pin usage
            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, // Pin usage
            0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0, // Pin usage (148)
            0x00, 0x64, // VDD_3V3B Current 100mA (2)
            0x00, 0x00, // VDD_5V Current 0mA (2)
            0x01, 0x90, // SYS_5V Current 400mA (2)
            0x05, 0xDC, // DC Supplied 1500mA (2)
            0x00, 0x00, // Placeholder for VDD_3V3B (AIN 7)
            0x00, 0x00, // Placeholder for BBB Current Consumption (AIN 3)
            0x00, 0x00, // Placeholder for LCD Voltage OFF (AIN 6)
            0x00, 0x00, // Placeholder for LCD Voltage ON (AIN 6)
            0x00, 0x00, // Placeholder for Radon Current Consumption OFF (AIN 2)
            0x00, 0x00 // Placeholder for Radon Current Consumption ON (AIN 2)
            };

        mem[244] = char((vdd3V3B >> 8) & 0x00FF);
        mem[245] = char(vdd3V3B & 0x00FF);
        mem[246] = char((bbbCurrent >> 8) & 0x00FF);
        mem[247] = char(bbbCurrent & 0x00FF);
        mem[248] = char((lcdOffVoltage >> 8) & 0x00FF);
        mem[249] = char(lcdOffVoltage & 0x00FF);
        mem[250] = char((lcdOnVoltage >> 8) & 0x00FF);
        mem[251] = char(lcdOnVoltage & 0x00FF);
        mem[252] = char((radonCurrent[0] >> 8) & 0x00FF);
        mem[253] = char(radonCurrent[0] & 0x00FF);
        mem[254] = char((radonCurrent[1] >> 8) & 0x00FF);
        mem[255] = char(radonCurrent[1] & 0x00FF);

        ofstream fs;
        fs.open("/sys/bus/i2c/devices/1-0054/eeprom");
        if (!fs.is_open()){
            perror("GPIO: write failed to open file ");
            return false;
        }
        fs.write(mem, 260);
        fs.close();

        return true;
    }

public:
    TestTask(QObject *parent = 0) : QObject(parent) {}

public slots:

    void run()
    {
        GPIO resetLine(22);
        resetLine.setDirection(OUTPUT);
        resetLine.setValue(HIGH);

        bool result = true;
        do
        {
            if (!(result = testVDD_3V3B())) break;
            sleep(20);
            if (!(result = testBBB_CurrentConsumption(200, 600))) break;

            if (!(result = testLcdOff_Voltage())) break;
            GPIO lcdBacklight(50);
            lcdBacklight.setDirection(OUTPUT);
            lcdBacklight.setValue(HIGH);
            sleep(1);
            result = testLcdOn_Voltage();
            lcdBacklight.setValue(LOW);
            if (!result) break;

            if (!(result = testRadon_CurrentConsumption(0, 50, 0))) break;
            GPIO radonPower(7);
            radonPower.setDirection(OUTPUT);
            radonPower.setValue(HIGH);
            sleep(3);
            result = testRadon_CurrentConsumption(350, 600, 1);
            radonPower.setValue(LOW);
            if (!result) break;

            if (!(result = program_EEPROM())) break;
        } while (false);

        sleep(1);
        emit resultReady(result);
        emit finished();
    }

signals:
    void resultReady(bool result);
    void finished();
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestTask *testt = new TestTask(&app);
    LedThread ledt;
    QObject::connect(testt, &TestTask::resultReady, &ledt, &LedThread::handleResult);
    QObject::connect(testt, &TestTask::finished, &app, QCoreApplication::quit);
    ledt.start();
    QTimer::singleShot(0, testt, SLOT(run()));
    app.exec();
    ledt.wait();
    return 0;
}

#include "main.moc"

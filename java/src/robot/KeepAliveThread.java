package robot;

import java.lang.Thread;

import manager.RobotState;

public class KeepAliveThread extends Thread
{
    RobotState robotState;
    
    public KeepAliveThread(RobotState robotState)
    {
        this.robotState = robotState;
    }

    @Override
    public void run()
    {
        while (true) {
            robotState.getNetworker().sendCommand("KEEP ALIVE", (byte) 0);

            try { Thread.sleep(100); } catch (InterruptedException ex) { }
        }
    }
}

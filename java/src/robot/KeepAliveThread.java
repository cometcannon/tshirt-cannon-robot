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
        
    }
}

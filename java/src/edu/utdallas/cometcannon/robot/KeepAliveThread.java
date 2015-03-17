package edu.utdallas.cometcannon.robot;

import edu.utdallas.cometcannon.manager.RobotState;
import edu.utdallas.cometcannon.robot.command.KeepAliveCommand;

public class KeepAliveThread implements Runnable
{
    RobotState robotState;
    
    public KeepAliveThread(RobotState robotState)
    {
        this.robotState = robotState;
    }

    @Override
    public void run()
    {
        System.out.println("started the keep alive");
        while (true) {
            robotState.addNewCommand(new KeepAliveCommand());

            try { Thread.sleep(100); } catch (InterruptedException ex) { }
        }
    }
}

package edu.utdallas.cometcannon.robot;

import edu.utdallas.cometcannon.robot.command.*;
import java.util.concurrent.*;

public class PressureCommandThread implements Runnable 
{
    private boolean keepAlive = true;
    private ArrayBlockingQueue<Command> robotCommandQueue;

    public PressureCommandThread(ArrayBlockingQueue<Command> robotCommandQueue)
    {
        this.robotCommandQueue = robotCommandQueue;
    }

    public void Terminate()
    {
        keepAlive = false;
    }

    public void run()
    {
        while(keepAlive)
        {
            robotCommandQueue.offer(new SendPressureCommand());
            try { Thread.sleep(500); } catch (InterruptedException ex) { }
        }
    }
}

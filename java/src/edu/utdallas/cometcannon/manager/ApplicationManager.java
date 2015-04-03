package edu.utdallas.cometcannon.manager;

import java.util.concurrent.*;

import edu.utdallas.cometcannon.robot.command.*;
import edu.utdallas.cometcannon.gui.*;

public class ApplicationManager
{
    private static ApplicationManager instance = null;
    
    private ArrayBlockingQueue<Command> robotCommandQueue;

    private static ApplicationManager create()
    {
        if (instance == null)
            instance = new ApplicationManager();
        
        return instance;
    }
    
    private ApplicationManager()
    {
        robotCommandQueue = new ArrayBlockingQueue<Command>(100);
    }
    
    private ArrayBlockingQueue<Command> getRobotCommandQueue()
    {
        return robotCommandQueue;
    }
    
    public static void main(String[] args)
    {
        ApplicationManager manager = ApplicationManager.create();
        MainFrame mainFrame = new MainFrame(manager.getRobotCommandQueue());
    }
}

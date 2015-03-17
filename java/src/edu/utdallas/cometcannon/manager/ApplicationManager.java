package edu.utdallas.cometcannon.manager;

public class ApplicationManager
{
    RobotManager robotManager;
    UIManager uiManager;
    
    ApplicationState applicationState;
    RobotState robotState;

    private ApplicationManager()
    {
        applicationState = new ApplicationState();
        robotState = new RobotState();
        
        robotManager = new RobotManager(applicationState, robotState);
        uiManager = new UIManager(applicationState, robotState);
        
        uiManager.run();
        robotManager.run();
    }
    
    public static void main(String[] args)
    {
        ApplicationManager manager = new ApplicationManager();
    }
}

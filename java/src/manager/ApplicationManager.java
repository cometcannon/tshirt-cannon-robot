package manager;

public class ApplicationManager
{
    RobotManager robotManager;
    UIManager uiManager;
    
    ApplicationState applicationState;
    RobotState robotState;
    UIState uiState;
    
    private ApplicationManager()
    {
        applicationState = new ApplicationState();
        robotState = new RobotState();
        uiState = new UIState();
        
        robotManager = new RobotManager(applicationState, robotState, uiState);
        uiManager = new UIManager(applicationState, robotState, uiState);
    }
    
    public static void main(String[] args)
    {
        ApplicationManager manager = new ApplicationManager();
    }
}

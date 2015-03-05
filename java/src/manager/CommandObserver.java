package manager;

import java.util.Observer;
import java.util.Observable;

import robot.Networker;

public class CommandObserver implements Observer
{
    private ObservableCommand command;
    private RobotState robotState;
    
    public CommandObserver(RobotState robotState, ObservableCommand command)
    {
        this.command = command; 
        this.robotState = robotState;
    }

    public void update(Observable o, Object arg)
    {
        if (o == command) {
            robotState.getNetworker().sendCommand(command.getType(), (byte) command.getMagnitude());
        }
    }
}

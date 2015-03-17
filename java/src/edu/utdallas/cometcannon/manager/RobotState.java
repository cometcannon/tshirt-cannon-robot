package edu.utdallas.cometcannon.manager;

import edu.utdallas.cometcannon.robot.command.Command;

import java.util.Queue;
import java.util.LinkedList;

public class RobotState
{
    private Queue<Command> commandQueue;
    
    protected RobotState()
    {
        commandQueue = new LinkedList<>();
    }

    public synchronized void addNewCommand(Command newCommand)
    {
        commandQueue.offer(newCommand);
    }
    
    public synchronized boolean isCommandAvailable()
    {
        return !commandQueue.isEmpty();
    }
    
    public synchronized Command pollNextCommand()
    {
        return commandQueue.poll();
    }
}

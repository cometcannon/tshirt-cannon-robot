package edu.utdallas.cometcannon.gui;

import javax.swing.JFrame;

import edu.utdallas.cometcannon.manager.RobotState;

public class MainFrame extends JFrame
{
    RobotState robotState;
    
    public MainFrame(RobotState robotState)
    {
        this.robotState = robotState;

        DebugPanel debug = new DebugPanel(robotState);
        add(debug);
        
        setSize(400, 250);
        setLocationRelativeTo(null);
        setVisible(true);
    }
}
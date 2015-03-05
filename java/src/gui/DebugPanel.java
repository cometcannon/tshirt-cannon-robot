package gui;

import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JComboBox;
import javax.swing.JButton;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import manager.RobotState;

import robot.command.MotorCommand;
import robot.command.KillRobotCommand;
import robot.command.MoveDirectionCommand;
import robot.command.RotateCommand;

public class DebugPanel extends JPanel
{
    RobotState robotState;

    JSlider slider;
    JComboBox comboBox;
    JButton sendButton;
    JButton killButton;
    
    protected DebugPanel(RobotState robotState)
    {
        this.robotState = robotState;

        slider = new MagnitudeSlider();
        comboBox = new CommandComboBox();
        sendButton = new JButton("SEND");
        killButton = new JButton("KILL");

        sendButton.addActionListener(new SendListener());
        killButton.addActionListener(new KillListener());

        add(slider);
        add(comboBox);
        add(sendButton);
        add(killButton);
    }

    class SendListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            String commandType = (String) comboBox.getSelectedItem();
            int magnitude = slider.getValue();

            if (commandType.equalsIgnoreCase("MOTOR 0"))
                robotState.addNewCommand(new MotorCommand(0, magnitude));
            
            if (commandType.equalsIgnoreCase("MOTOR 1"))
                robotState.addNewCommand(new MotorCommand(1, magnitude));
            
            if (commandType.equalsIgnoreCase("MOTOR 2"))
                robotState.addNewCommand(new MotorCommand(2, magnitude));
            
            if (commandType.equalsIgnoreCase("MOTOR 3"))
                robotState.addNewCommand(new MotorCommand(3, magnitude));
            
            if (commandType.equalsIgnoreCase("FORWARD") ||
                commandType.equalsIgnoreCase("BACKWARD") ||
                commandType.equalsIgnoreCase("RIGHT") ||
                commandType.equalsIgnoreCase("LEFT"))
                robotState.addNewCommand(new MoveDirectionCommand(commandType, magnitude));
            
            if (commandType.equalsIgnoreCase("ROTATE CCW"))
                robotState.addNewCommand(new RotateCommand("CCW", magnitude));
            
            if (commandType.equalsIgnoreCase("ROTATE CW"))
                robotState.addNewCommand(new RotateCommand("CW", magnitude));
        }
    }

    class KillListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            robotState.addNewCommand(new KillRobotCommand());
        }
    }
}

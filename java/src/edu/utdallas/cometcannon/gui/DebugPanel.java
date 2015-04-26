package edu.utdallas.cometcannon.gui;

import javax.swing.*;
import java.awt.event.*;
import java.util.concurrent.*;

import edu.utdallas.cometcannon.robot.command.*;

public class DebugPanel extends JPanel
{
    ArrayBlockingQueue<Command> robotCommandQueue;

    private JSlider slider;
    private JComboBox comboBox;
    private JButton sendButton;
    private JButton killButton;

    protected DebugPanel(ArrayBlockingQueue<Command> robotCommandQueue)
    {
        this.robotCommandQueue = robotCommandQueue;

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
                robotCommandQueue.offer(new MotorCommand(0, magnitude));

            if (commandType.equalsIgnoreCase("MOTOR 1"))
                robotCommandQueue.offer(new MotorCommand(1, magnitude));

            if (commandType.equalsIgnoreCase("MOTOR 2"))
                robotCommandQueue.offer(new MotorCommand(2, magnitude));

            if (commandType.equalsIgnoreCase("MOTOR 3"))
                robotCommandQueue.offer(new MotorCommand(3, magnitude));

            if (commandType.equalsIgnoreCase("FORWARD") ||
                commandType.equalsIgnoreCase("BACKWARD") ||
                commandType.equalsIgnoreCase("RIGHT") ||
                commandType.equalsIgnoreCase("LEFT"))
                robotCommandQueue.offer(new MoveDirectionCommand(commandType, magnitude));

            if (commandType.equalsIgnoreCase("ROTATE CCW"))
                robotCommandQueue.offer(new RotateCommand("CCW", magnitude));

            if (commandType.equalsIgnoreCase("ROTATE CW"))
                robotCommandQueue.offer(new RotateCommand("CW", magnitude));

            if (commandType.equalsIgnoreCase("FIRE"))
                robotCommandQueue.offer(new FireCannonCommand());
        }
    }

    class KillListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            robotCommandQueue.offer(new KillRobotCommand());
        }
    }
}

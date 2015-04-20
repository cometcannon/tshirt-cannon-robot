package edu.utdallas.cometcannon.gui;

import javax.swing.*;
import java.util.concurrent.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

import edu.utdallas.cometcannon.robot.*;
import edu.utdallas.cometcannon.robot.command.*;
import edu.utdallas.cometcannon.robot.response.*;

public class MainFrame extends JFrame
{
    public static final int WIDTH = 400;
    public static final int HEIGHT = 250;

    private ArrayBlockingQueue<Command> robotCommandQueue;
    private ArrayBlockingQueue<Response> robotResponseQueue;

    public MainFrame(ArrayBlockingQueue<Command> robotCommandQueue,
                     ArrayBlockingQueue<Response> robotResponseQueue)
    {
        this.robotCommandQueue = robotCommandQueue;
        this.robotResponseQueue = robotResponseQueue;
        JMenuBar menuBar = buildMenuBar();
        setJMenuBar(menuBar);

        buildPanels();

        setSize(WIDTH, HEIGHT);
        setResizable(false);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setVisible(true);
    }

    private void buildPanels()
    {
        VelocityPanel velocityPanel = new VelocityPanel();
        PressurePanel pressurePanel = new PressurePanel();
        StatusPanel statusPanel = new StatusPanel();

        JPanel visualPanel = new JPanel();
        visualPanel.setLayout(new GridLayout(1,2));
        visualPanel.add(velocityPanel);
        visualPanel.add(pressurePanel);

        setLayout(new BorderLayout());

        add(visualPanel, BorderLayout.CENTER);
        add(statusPanel, BorderLayout.SOUTH);
    }

    private JMenuBar buildMenuBar()
    {
        JMenuBar menuBar = new JMenuBar();
        MenuItemListener menuListener = new MenuItemListener();

        JMenu fileMenu = buildFileMenu(menuListener);
        JMenu viewMenu = buildViewMenu(menuListener);

        menuBar.add(fileMenu);
        menuBar.add(viewMenu);

        return menuBar;
    }

    private JMenu buildFileMenu(MenuItemListener listener)
    {
        JMenu fileMenu = new JMenu("File");

        JMenuItem connectToRobotMenuItem = new JMenuItem("Connect to Robot");
        JMenuItem connectToControllerMenuItem = new JMenuItem("Connect to Controller");
        JMenuItem exitMenuItem = new JMenuItem("Exit");

        connectToRobotMenuItem.addActionListener(listener);
        connectToControllerMenuItem.addActionListener(listener);
        exitMenuItem.addActionListener(listener);

        fileMenu.add(connectToRobotMenuItem);
        fileMenu.add(connectToControllerMenuItem);
        fileMenu.add(exitMenuItem);

        return fileMenu;
    }

    private JMenu buildViewMenu(MenuItemListener listener)
    {
        JMenu viewMenu = new JMenu("View");

        JMenuItem videoStreamMenuItem = new JMenuItem("Video Stream");
        JMenuItem debugPanelMenuItem = new JMenuItem("Debug Panel");

        videoStreamMenuItem.addActionListener(listener);
        debugPanelMenuItem.addActionListener(listener);

        viewMenu.add(videoStreamMenuItem);
        viewMenu.add(debugPanelMenuItem);

        return viewMenu;
    }

    class MenuItemListener implements ActionListener
    {
        @Override
        public void actionPerformed(ActionEvent e)
        {
            JMenuItem source = (JMenuItem) e.getSource();
            String sourceText = source.getText();

            switch (sourceText) {
                case "Connect to Robot":
                    Networker net = new Networker(robotCommandQueue,
                                                  robotResponseQueue);
                    Thread netThread = new Thread(net);
                    netThread.start();
                    break;
                case "Connect to Controller":
                    RemoteController rc = new RemoteController(robotCommandQueue);
                    Thread rcThread = new Thread(rc);
                    rcThread.start();
                    break;
                case "Exit":
                    System.exit(0);
                    break;
                case "Video Stream":
                    startVideoStream();
                   break;
                case "Debug Panel":
                    System.out.println("Starting Pressure Command Thread");

                    PressureCommandThread pc = new PressureCommandThread(robotCommandQueue);
                    Thread pcThread = new Thread(pc);
                    pcThread.start();

                    System.out.println("Open up new frame with debug panel here");
                    break;
            }
        }

        private void startVideoStream()
        {
            try {
                String[] cmd = {"/bin/sh",
                                "-c",
                                "nc 192.168.240.3 5001 | mplayer -gui -fps 31 -cache 1024"};
                Runtime runtime = Runtime.getRuntime();
                Process proc = runtime.exec(cmd);
            } catch (IOException ex) {
                System.out.println(ex);
            }
         }
    }
}

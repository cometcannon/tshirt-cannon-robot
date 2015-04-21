package edu.utdallas.cometcannon.gui;

import javax.swing.*;
import javax.swing.border.*;
import java.util.concurrent.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

import edu.utdallas.cometcannon.robot.*;
import edu.utdallas.cometcannon.robot.command.*;
import edu.utdallas.cometcannon.robot.response.*;

public class MainFrame extends JFrame
{
    public static int WIDTH = 400;
    public static int HEIGHT = 250;

    public static int V_X;
    public static int V_Y;

    private ArrayBlockingQueue<Command> robotCommandQueue;
    private ArrayBlockingQueue<Response> robotResponseQueue;

    private VelocityPanel velocityPanel;
    private StatusPanel statusPanel;
    private PressurePanel pressurePanel;

    public MainFrame(ArrayBlockingQueue<Command> robotCommandQueue,
                     ArrayBlockingQueue<Response> robotResponseQueue)
    {
        this.robotCommandQueue = robotCommandQueue;
        this.robotResponseQueue = robotResponseQueue;
        JMenuBar menuBar = buildMenuBar();
        setJMenuBar(menuBar);

        buildPanels();

        addComponentListener(new ResizeListener());

        setSize(WIDTH, HEIGHT);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setVisible(true);

        try {Thread.sleep(2000);} catch (InterruptedException ex) {}
        statusPanel.updateStatus("<html>Status: <font color='green'>Oh wait, never mind. We're good.</font></html>");

        try {Thread.sleep(2000);} catch (InterruptedException ex) {}
        statusPanel.updateStatus("<html>Status: <font color='green'>OK</font></html>");
    }

    private void buildPanels()
    {
        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new BorderLayout());

        final int BORDER_HEIGHT = (int) (MainFrame.HEIGHT * 0.04);
        Border emptyBorder = BorderFactory.createEmptyBorder(BORDER_HEIGHT,
                                                             BORDER_HEIGHT,
                                                             BORDER_HEIGHT,
                                                             BORDER_HEIGHT);
        mainPanel.setBorder(emptyBorder);

        velocityPanel = new VelocityPanel(robotCommandQueue);
        pressurePanel = new PressurePanel();
        statusPanel = new StatusPanel();

        JPanel visualPanel = new JPanel();
        visualPanel.setLayout(new GridLayout(1,2));
        visualPanel.add(velocityPanel);
        visualPanel.add(pressurePanel);

        setLayout(new BorderLayout());

        mainPanel.add(visualPanel, BorderLayout.CENTER);
        mainPanel.add(statusPanel, BorderLayout.SOUTH);

        add(mainPanel);
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

    class ResizeListener extends ComponentAdapter
    {
        public void componentResized(ComponentEvent e)
        {
            WIDTH = getWidth();
            HEIGHT = getHeight();
            velocityPanel.updateRobotSize();
        }
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

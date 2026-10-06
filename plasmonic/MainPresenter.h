#pragma once

class MainWindow;

class MainPresenter
{
public:
    explicit MainPresenter(MainWindow* view);
    ~MainPresenter();

private:
    MainWindow* view;

    void onConnectRequested();
};
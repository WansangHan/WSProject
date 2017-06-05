<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class BattleCity extends CI_Controller {

    function __construct()
    {
        parent::__construct();
    }

    public function index()
    {
    }

    public function NewAccount()
    {
        $this->load->model('BattleCity_model');
        echo $this->BattleCity_model->NewAccount();
    }

    public function Login()
    {
        $this->load->model('BattleCity_model');
        echo $this->BattleCity_model->Login();
    }
}
?>
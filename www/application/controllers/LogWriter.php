<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class LogWriter extends CI_Controller {

    function __construct()
    {
        parent::__construct();
    }

    public function index()
    {
    }

    public function LogWriter()
    {
        $this->load->model('LogWriter_model');
        echo $this->LogWriter_model->ErrorWriter();
    }
}
?>
<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class LogWriter extends CI_Controller {

    function __construct()
    {
        parent::__construct();
    }

    public function index()
    {
        echo 'Index_Controller';
    }

    public function LogWriter()
    {
        echo 'LogWriter_model_Controller';
        $this->load->model('LogWriter_model');
        echo $this->LogWriter_model->ErrorWriter();
    }
}
?>
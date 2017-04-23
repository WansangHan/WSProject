<?php

class LogWriter_model extends CI_Model {

    function __construct()
    {
        parent::__construct();
        $this->load->database();
    }

    public function ErrorWriter()
    {
        $q1 = "
			SELECT INSERT_LOG_MESSAGE('WEB', 'INFO', 'ERRORWRITER') FROM DUAL
		";
        var_dump($this->db);
        log_message('debug', $q1);
        $result = $this->db->query($q1);

        return json_encode($result);
    }
}
?>
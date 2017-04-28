<?php

class LogWriter_model extends CI_Model {

    function __construct()
    {
        parent::__construct();
        $this->load->database();
    }

    public function ErrorWriter()
    {
        $input = $input = json_decode(file_get_contents('php://input'), true);

        $q1 = "
			SELECT INSERT_LOG_MESSAGE('".$input['cate']."', '".$input['level']."', '".$input['message']."') FROM DUAL
		";
        log_message('debug', $q1);
        $result = $this->db->query($q1);

        return json_encode($result);
    }
}
?>
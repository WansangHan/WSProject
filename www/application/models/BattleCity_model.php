<?php

class BattleCity_model extends CI_Model {

    function __construct()
    {
        parent::__construct();
        $this->load->database();
    }

    public function NewAccount()
    {
        $input = json_decode(file_get_contents('php://input'), true);

        $input['pw'] = password_hash($input['pw'], PASSWORD_BCRYPT);

        $this->db->trans_start();

        $q1 = "call INSERT_USERINFO_MESSAGE('".$input['id']."', '".$input['pw']."', '".$input['ml']."', @isSuccess, @text);";

        $this->db->query($q1);
        log_message('debug', $q1);
        $query = $this->db->query('select @isSuccess as isSuccess, @text as text');
        $result = $query->result_array();

        $this->db->trans_complete();

        $isSuccess = $result[0]['isSuccess'];
        $text = $result[0]['text'];

        $output = array(
            'isSuccess' => $isSuccess,
            'text' => $text
        );
        log_message('debug', $isSuccess);
        return json_encode($output);
    }

    public function Login()
    {
        $input = json_decode(file_get_contents('php://input'), true);


        $q1 = "
			SELECT USER_IDNT, USER_PSWD, USER_MAIL
            FROM USER_INFO
            WHERE 1=1
            AND USER_IDNT='".$input['id']."'
		";

        $query = $this->db->query($q1);
        log_message('debug', $q1);
        $result = $query->result_array();

        if($query->num_rows())
        {
            if(password_verify($input['pw'], $result[0]['USER_PSWD']))
            {
                $isSuccess = 'success';
            }
            else
            {
                $isSuccess = 'fail';
            }
        }
        else
        {
            $isSuccess = 'fail';
        }

        return $isSuccess;
    }
}
?>
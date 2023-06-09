#include "SQLlite3.hpp"


SQLite3::SQLite3(const std::string& path){
    this->prep_stmt = nullptr;
    
    int rc;
    rc = sqlite3_open(path.c_str(), &this->db);
    
    if(rc != SQLITE_OK){
        _PF_DB_OPEN_;
        this->db_open = false;
        return;
    }

    this->db_open = true;
}

SQLite3::~SQLite3(){
    int rc;
    rc = sqlite3_finalize(this->prep_stmt);
    if(rc != SQLITE_OK){
        _PF_STMT_CLOSE_;
    }

    if(this->db_open){
        rc = sqlite3_close(this->db);
        if(rc != SQLITE_OK){
            _PF_DB_CLOSE_;
        }
    }
}


std::string SQLite3::read_row(sqlite3_stmt* stmt) const{
    std::string result;
    for(int i=0; i<sqlite3_data_count(stmt); i++){
        switch (sqlite3_column_type(stmt, i)){
        case SQLITE_INTEGER:
            result += std::to_string(sqlite3_column_int(stmt, i));
            break;
        case SQLITE_FLOAT:
            result += std::to_string(sqlite3_column_double(stmt, i));
            break;
        case SQLITE_TEXT:
            result += reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            break;
        case SQLITE_BLOB:
            result += "{BLOB}"; //  binary labge object
            break;
        case SQLITE_NULL:
            result += "NULL";
            break;
        default:
            result += "{unknown}"; // unknown data type
            break;
        }
        result += " | ";
    }
    if(result.size() > 0) 
        result = std::string("| ") + result;

    return result;
}


void SQLite3::prepare_query(const std::string& query){
    if(this->db_open){
        int rc;
        if(this->prep_started){
            rc = sqlite3_finalize(this->prep_stmt);
            if(rc != SQLITE_OK){
                _PF_STMT_CLOSE_;
            }
        }
        rc = sqlite3_prepare_v2(this->db, query.c_str(), -1, &this->prep_stmt, nullptr);
        if(rc != SQLITE_OK){
            _PF_PREPARE_;
            this->prep_started = false;
            return;
        }
        this->prep_started = true;
    }
    else{
        _PF_SKIPPED_OPEN_;
        this->prep_started = false;
    }
}


void SQLite3::bind_int(int pos, int param){
    if(this->prep_started){
        int rc;
        rc = sqlite3_bind_int(this->prep_stmt, pos, param);
        if(rc != SQLITE_OK){
            _PF_BIND_;
        }
    }
    else{
        _PF_SKIPPED_PREP_;
    }
}
void SQLite3::bind_double(int pos, double param){
    if(this->prep_started){
        int rc = sqlite3_bind_double(this->prep_stmt, pos, param);
        if(rc != SQLITE_OK){
            _PF_BIND_;
        }
    }
    else{
        _PF_SKIPPED_PREP_;
    }
}
void SQLite3::bind_text(int pos, std::string param){
    if(this->prep_started){
        // SQLITE_TRANSIENT makes that string is copied
        int rc = sqlite3_bind_text(this->prep_stmt, pos, param.c_str(), -1, SQLITE_TRANSIENT); 
        if(rc != SQLITE_OK){
            _PF_BIND_;
        }
    }
    else{
        _PF_SKIPPED_PREP_;
    }
}
void SQLite3::bind_null(int pos){
    if(this->prep_started){
        int rc = sqlite3_bind_null(this->prep_stmt, pos);
        if(rc != SQLITE_OK){
            _PF_BIND_;
        }
    }
    else{
        _PF_SKIPPED_PREP_;
    }
}
void SQLite3::binds_clear(){
    sqlite3_clear_bindings(this->prep_stmt);
}


std::vector<std::string> SQLite3::execute_prepared_query(){
    std::vector<std::string> feedback;
    if(this->prep_started){
        int rc;
        
        rc = sqlite3_step(this->prep_stmt);
        while(rc == SQLITE_ROW){
            feedback.push_back(this->read_row(this->prep_stmt));
            rc = sqlite3_step(this->prep_stmt);
        }
        
        if(rc == SQLITE_DONE){
            if(feedback.size() > 0){
                std::string c_name = "|";
                for(int i=0; i<sqlite3_column_count(this->prep_stmt); i++){
                    c_name += std::string(sqlite3_column_name(this->prep_stmt, i)) + " | ";
                }
                feedback.insert(feedback.begin(), c_name);
            }

            rc = sqlite3_reset(this->prep_stmt);
            if(rc != SQLITE_OK){
                _PF_STMT_RESET_;
            }
        }
        else{
            _PF_STEP_;
        }
    }
    else{
        _PF_SKIPPED_PREP_;
    }
    return feedback;
}


void SQLite3::clear_prepared_query(){
    int rc = sqlite3_finalize(this->prep_stmt);
    if(rc != SQLITE_OK){
        _PF_STMT_CLOSE_;
    }
}
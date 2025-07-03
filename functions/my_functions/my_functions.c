/**
 * @file    my_functions.c
 * @brief   realizations of additional used fucntions
 * @author  Chi Xia
 */

#include "my_functions.h"

/**
 * @brief   implement matrix multiplication
 * @details specefically designed for hop matrix, which is a symmetric matrix
 * @param[in]   matrix_A    a pointer of matrix one, this should be an one-dimensional matrix [1*(dim^2)]
 * @param[in]   matrix_B    a pointer of matrix two, also a one-dimensional matrix [1*(dim^dim)]
 * @param[in]   dim         variable describes length or dimension of to be operated matrix
 * @param[out]   matrix_out  a pointer of resulting matrix
 */
void matrix_multiply(const unsigned char* matrix_A, const unsigned char* matrix_B, const unsigned char dim, unsigned char* const matrix_out){
    unsigned char temp_matrix[dim*dim];
    // initialize matrix
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            temp_matrix[i*dim+j] = 0;
        }
    }
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            for(int k=0; k<dim; k++){
                temp_matrix[i*dim + j] += (matrix_A[i*dim+k] * matrix_B[k*dim+j]);
            }
        }
    }
    // assign value to result
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            matrix_out[i*dim+j] = temp_matrix[i*dim+j];
        }
    }
}

/**
 * @brief       calculate adjacent matrix based on hop time
 * @param[in]   template    pointer of adjacent matrix whose hop time is 1
 * @param[out]   target      pointer of resulting matrix
 * @param[in]   dim         length or dimension of matrix
 * @param[in]   hop_time    number of maximum hop
 */
void hop_matrix(const unsigned char* template, unsigned char* target, const unsigned char dim, const unsigned char hop_time){
    if(hop_time == 1){ // return original matrix
        for(int i=0; i<dim*dim; i++)target[i] = template[i];
        return;
    }else if (hop_time == 0) {// hop time should be larger than 0
        return;
    }

    //calculate matrix based on hop_time
    const unsigned char* matrixA = template;
    const unsigned char* matrixB = template;
    for(int i=1;i<hop_time;i++){
        matrix_multiply(matrixA, matrixB, dim, target);
        matrixA = target;
    }
}

/**
 * @brief   rank the adjacent matrix based on the number of possible paths
 * @param[in]   matrix      pointer of input matrix, 1*(dim*dim)
 * @param[out]   ordered     pointer of resulting matrix, 1*dim
 * @param[in]   dim         dimension of matrix
 */
void ordering(const unsigned char* matrix, unsigned char* const ordered, const unsigned char dim) {
    unsigned char operating_matrix[dim][2], ord[dim];
    // sum every row to get the number of possible paths
    // also indicate the numbering of every row.
    for (int i = 0; i < dim; i++) {
        operating_matrix[i][0] = 0;
        operating_matrix[i][1] = i;
        for (int j = 0; j < dim; j++) {
            operating_matrix[i][0] += matrix[i*dim + j];
        }
    }
    // apply bubble sort
    for (int i = 0; i < dim - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < dim - 1 - i; j++) {
            if (operating_matrix[j][0] < operating_matrix[j + 1][0]) {
                unsigned char temp = operating_matrix[j][0];
                operating_matrix[j][0] = operating_matrix[j + 1][0];
                operating_matrix[j + 1][0] = temp;
                temp = operating_matrix[j][1];
                operating_matrix[j][1] = operating_matrix[j + 1][1];
                operating_matrix[j + 1][1] = temp;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
    // assign ranking to result
    for (int i = 0; i < dim; i++) {
        ordered[i] = operating_matrix[i][1];
    }
}

/**
 * @brief   check out whether head candidates can connect directly to master
 * @param[in]   master_matrix   pointer of matrix, which describes the connections between nodes and master
 * @param[in]   dim             dimension
 * @param[in]   candidate_cluster       pointer of matrix, which includes the numberings of candidates
 * @param[in]   num             number of candidates
 * @return      number of candidates that can connect to master
 */
unsigned char if_connect_master(const unsigned char* master_matrix, const unsigned char dim, const unsigned char* candidate_cluster, const unsigned char num) {
    unsigned char flag = 0;
    for(int i=0; i<num; i++) {
        if (candidate_cluster[i]<dim) {
            if (master_matrix[candidate_cluster[i]] == 1) {
                flag ++;
            }
        }
    }
    return flag;
}

/**
 * @brief   check out whether the candidates can connect to each other, but this function only considers when the number of candidates is 3
 * @param[in]   tempalte    pointer of matrix, which is the adjacent matrix when hop time is 1
 * @param[in]   dim         dimension
 * @param[in]   candidate_cluster   pointer of matrix, which contains numberings of candidates
 * @param[in]   num         number of candidates
 * @return      number of edges among these candidates
 */
unsigned char if_connect_each(const unsigned char* template, const unsigned char dim, const unsigned char* candidate_cluster, const unsigned char num) {
    const unsigned char candidate1 = candidate_cluster[0];
    const unsigned char candidate2 = candidate_cluster[1];
    const unsigned char candidate3 = candidate_cluster[2];
    unsigned char flag = 0;
    if (template[candidate1*dim + candidate2] == 1)flag ++;
    if (template[candidate2*dim + candidate3] == 1)flag ++;
    if (template[candidate3*dim + candidate1] == 1)flag ++;

    return flag;
}

/**
 * @brief   function, which is used to choose heads.
 * @details The choosing scheme is based on three criterias
 *          1. The number of possible paths, when hop time is 1, 2 and 3
 *          2. Whether heads can directly connect to master
 *          3. Wheter heads can connect to each other without routing message by non-head nodes
 * @param[in]   hop_template    pointer of adjacent matrix, whose hop time is 1, 1*(dim^2)
 * @param[in]   num_cluster     aimed number of head, only implement 3
 * @param[in]   dim             dimension
 * @param[in]   master          pointer of matrix, which describes the connections between nodes and master, 1*dim
 * @param[out]   res             pointer of matrix, whose content is the numbering of chosen head, 1*num_cluster
 */
void cluster_head_choose(const unsigned char* hop_template, const unsigned num_cluster, const unsigned char dim, const unsigned char* master, unsigned char* const res) {
    unsigned char matrix_hop1[dim*dim], matrix_hop2[dim*dim], matrix_hop3[dim*dim];
    // hop1
    hop_matrix(hop_template, matrix_hop1, dim, 1);
    // hop2
    hop_matrix(hop_template, matrix_hop2, dim, 2);
    // hop3
    hop_matrix(hop_template, matrix_hop3, dim, 3);

    unsigned char final_value_matrix[dim*dim];
    // calculate a weighted adjacent matrix, used to order
    memset(&final_value_matrix, 0, dim*dim*sizeof(unsigned char));
    for (int i=0;i<dim; i++) {
        for (int j=0;j<dim;j++) {
            const unsigned char weight[3] = {4, 2, 1};
            final_value_matrix[i*dim + j] = weight[0]*matrix_hop1[i*dim + j] + weight[1]*matrix_hop2[i*dim + j] + weight[0]*matrix_hop3[i*dim + j];
        }
    }
    unsigned char ordered[dim];
    ordering(final_value_matrix, ordered, dim);

    // map numberings of pre-defined head-group to ordered numberings
    unsigned char combination[10][num_cluster];
    for(int i=0; i<10; i++) {
        for(int j=0; j<num_cluster; j++) {
            combination[i][j] = ordered[combination3[i][j]];
        }
    }

    // consider other two criterias
    unsigned char chosen_cluster[num_cluster];
    unsigned char to_master_matrix[10] = {0};
    // only consider choosing 3 clusters
    for (int i=0;i<10;i++) {
        to_master_matrix[i] = if_connect_master(master, dim, combination[i], num_cluster);
    }

    unsigned char to_each_matrix[10] = {0};
    for (int i=0;i<10;i++) {
        to_each_matrix[i] = if_connect_each(hop_template, dim, combination[i], num_cluster);
    }

    // sum up scores gained based on above two criterias
    unsigned char to_chosen_matrix[10] = {0};
    for (int i=0;i<10;i++) {
        to_chosen_matrix[i] = to_master_matrix[i] + to_each_matrix[i];
    }

    // choose a combination based on the scores
    unsigned char target_grade = 6; //3+3 is the maximum value, means 3 nodes can connect to master as well as each
    for (int i=0;i<10;i++) {
        if (to_chosen_matrix[i] >= target_grade && to_master_matrix[i] >= 0) {
            for (int j=0;j<num_cluster;j++) {
                chosen_cluster[j] = combination[i][j];
            }
            break;
        }
        target_grade --;
        if (target_grade == 0) {
            for (int j=0;j<num_cluster;j++) {
                chosen_cluster[j] = combination[0][j];
            }
            break;
        }
    }
    for (int i=0;i<num_cluster;i++) {
        res[i] = chosen_cluster[i];
    }
}
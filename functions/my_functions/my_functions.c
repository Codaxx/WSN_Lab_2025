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
 * @param[in]   battery     pointer of matrix, whose contents are the states of batteries
 */
void ordering(const unsigned char* matrix, unsigned char* const ordered, const unsigned char dim, const float* battery) {
    float operating_matrix[dim][2];
    for (int i = 0; i < dim; i++) {
        operating_matrix[i][0] = 0;
        operating_matrix[i][1] = (float)i;
        for (int j = 0; j < dim; j++) {
            if (i==j)
                operating_matrix[i][0] += 0;
            else
                operating_matrix[i][0] += (float)matrix[i*dim + j] * battery[i];
        }
    }
    for (int i = 0; i < dim - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < dim - 1 - i; j++) {
            if (operating_matrix[j][0] < operating_matrix[j + 1][0]) {
                float temp = operating_matrix[j][0];
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
    for (int i = 0; i < dim; i++) {
        ordered[i] = (unsigned char)operating_matrix[i][1];
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
 * @param[in]   battery     pointer of matrix, whose contents are the states of batteries
 * @param[out]   res             pointer of matrix, whose content is the numbering of chosen head, 1*num_cluster
 */
void cluster_head_choose(const unsigned char* hop_template, const unsigned num_cluster, const unsigned char dim, const unsigned char* master, const float* battery , unsigned char* const res) {
    unsigned char matrix_hop1[dim*dim], matrix_hop2[dim*dim], matrix_hop3[dim*dim];
    // hop1
    hop_matrix(hop_template, matrix_hop1, dim, 1);
    // hop2
    hop_matrix(hop_template, matrix_hop2, dim, 2);
    // hop3
    hop_matrix(hop_template, matrix_hop3, dim, 3);

    unsigned char final_value_matrix[dim*dim];
    memset(&final_value_matrix, 0, dim*dim*sizeof(unsigned char));
    for (int i=0;i<dim; i++) {
        for (int j=0;j<dim;j++) {
            const unsigned char weight[3] = {8, 4, 2};
            final_value_matrix[i*dim + j] = weight[0]*value_regularization(matrix_hop1[i*dim + j], 1) + weight[1]*value_regularization(matrix_hop2[i*dim + j],2 ) + weight[2]*value_regularization(matrix_hop3[i*dim + j], 3);
        }
    }
    unsigned char ordered[dim];
    ordering(final_value_matrix, ordered, dim, battery);
    unsigned char combination[10][num_cluster];
    for(int i=0; i<10; i++) {
        for(int j=0; j<num_cluster; j++) {
            combination[i][j] = ordered[combination3[i][j]];
        }
    }

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

    unsigned char to_chosen_matrix[10] = {0};
    for (int i=0;i<10;i++) {
        to_chosen_matrix[i] = to_master_matrix[i] + to_each_matrix[i];
    }
    unsigned char target_grade = 6; //3+3 is the maximum value, means 3 nodes can connect to master as well as each
    for (int i=0;i<10;i++) {
        if (to_chosen_matrix[i] >= target_grade && to_master_matrix[i]) {
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

float num_allocated(const unsigned char* allocated, const unsigned char dim) {
    float num = 0;
    for(int i=0; i<dim; i++) {
        num += (float)allocated[i];
    }
    return num;
}

int greatest_value_index(const float* matrix, const int length) {
    int index = 0;
    float temp = matrix[0];
    for(int i=1; i<length; i++) {
        if (matrix[i] >= temp) {
            temp = matrix[i];
            index = i;
        }
    }
    return index;
}

void group_selection(const unsigned char num_cluster, const unsigned char* cluster, const unsigned char dim, const unsigned char* hop_template, const unsigned char* master, const float* battery, unsigned char* const res) {
    unsigned char matrix_hop1[dim*dim];
    unsigned char connection_summary[dim];
    memset(connection_summary, 0, dim*sizeof(unsigned char));
    unsigned char node_allocation[num_cluster][dim];
    memset(node_allocation, 0, num_cluster*dim*sizeof(unsigned char));

    // calculate the number of hops mainly for cluster's head
    unsigned char all_hop_template[(dim+1)*(dim+1)];
    memset(all_hop_template, 0, (dim+1)*(dim+1)*sizeof(unsigned char));
    for(int i=0; i<dim+1; i++) {
        for(int j=0; j<dim+1; j++) {
            if (i == 0) {
                if (j == 0) {
                    all_hop_template[i*(dim+1) + j] = 0;
                }else {
                    all_hop_template[i*(dim+1) + j] = master[j-1];
                }
            }else {
                if (j == 0) {
                    all_hop_template[i*(dim+1) + j] = master[i-1];
                }else {
                    all_hop_template[i*(dim+1) + j] = hop_template[(i-1)*dim + j-1];
                }
            }
        }
    }

    unsigned char head_2_master_cost[num_cluster];
    memset(head_2_master_cost, 0, num_cluster*sizeof(unsigned char));
    unsigned char all_hop_matrix[(dim+1)*(dim+1)];
    memset(all_hop_matrix, 0, num_cluster*sizeof(unsigned char));
    unsigned char flag = 1, hop_num = 1;
    while (flag) {
        flag = 0;
        hop_matrix(all_hop_template, all_hop_matrix, dim+1, hop_num);
        for (int i=0; i<num_cluster; i++) {
            if (all_hop_matrix[cluster[i]+1] > 0 && head_2_master_cost[i] == 0) {
                head_2_master_cost[i] = hop_num;
            }
        }
        for (int i=0; i<num_cluster; i++) {
            if (head_2_master_cost[i] == 0) {
                flag = 1;
            }
        }
        hop_num ++;
    }

    // hop1
    hop_matrix(hop_template, matrix_hop1, dim, 1);
    for(int i=0; i<dim; i++) {
        for(int j=0; j<num_cluster; j++) {
            connection_summary[i] += matrix_hop1[i + dim*cluster[j]];
        }
    }

    for(int k=1; k<=num_cluster; k++) {
        for(int i=0; i<dim; i++) {
            float head_value[num_cluster];
            memset(head_value, 0, sizeof(float)*num_cluster);
            for (int j=0; j<num_cluster; j++) {
                if (matrix_hop1[i + dim*cluster[j]]*k == connection_summary[i] && connection_summary[i] != 0) {
                    head_value[j] = battery[cluster[j]]/(num_allocated(node_allocation[j], dim)+1)/(float)head_2_master_cost[j];
                }
            }
            if (connection_summary[i] == k) {
                node_allocation[greatest_value_index(head_value, num_cluster)][i] = 1;
            }
        }
    }
    // assign value to res
    for(int i=0; i<num_cluster; i++) {
        for(int j=0; j<dim; j++) {
            res[i*dim + j] = node_allocation[i][j];
        }
    }

}

void from_D2matrix_to_D1matrix(const unsigned char** D2matrix, const unsigned char D2dim, unsigned char* const D1matrix) {
    for(int i=0; i<D2dim; i++){
        for(int j=0; j<D2dim; j++){
            D1matrix[i*D2dim+j] = D2matrix[i][j];
        }
    }
}

void rssi_to_adjacent(const signed short* rssi_matrix, unsigned char* adjacent, const unsigned char dim){
    for (int i=0; i<dim; i++) {
        for (int j=0; j<dim; j++) {
            if (rssi_matrix[i*dim+j] != 0) {
                adjacent[i*dim+j] = 1;
            }else {
                adjacent[i*dim+j] = 0;
            }
        }
    }
}
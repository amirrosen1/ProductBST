#include "ProductBST.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

Node *new_node (char *name, int quantity);
char *allocate_and_cpy (const char *src);
Node *
add_product_helper (Node *origin_root, Node *root, char *name, int quantity);
int only_digit (const char *c);
Product *parse_product (char *line, Product *pc);
Node *find_min_value (Node *node);
void free_node (Node *node);

/**
 * This function build new node.
 * @param name : Name of the product.
 * @param quantity : The quantity of the product.
 * @return : The function return Null in case of a memory planing problem,
 * and if there is no problem, the function returns a pointer to the new node.
 */
Node *new_node (char *name, int quantity)
{
  Node *node = malloc (sizeof (Node));
  if (!node)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, ALLOCATION_FAILED);
    return NULL;
  }
  node->right_child = NULL;
  node->left_child = NULL;
  if (!(node->product.name = allocate_and_cpy (name)))
  {
    free (node);
    node = NULL;
    return NULL;
  }
  node->product.quantity = quantity;
  return node;
}

/**
 * The function allocates space to a char and copies it there.
 * @param src : Constant char.
 * @return : The function return Null in case of a memory planing problem,
 * and if there is no problem, the function returns a pointer to the
 * "destination char".
 */
char *allocate_and_cpy (const char *src)
{
  char *dest = malloc (strlen (src) + 1);
  if (!dest)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, ALLOCATION_FAILED);
    return NULL;
  }
  strcpy (dest, src);
  return dest;
}

/**
 * Auxiliary function to add_product.
 * @param origin_root : The root of the tree (remains fixed).
 * @param root : A changing node(recursion works on it).
 * @param name of the product.
 * @param quantity of the product.
 * @return Null if the tree is empty. If the name exists in the tree,
 * the function returns the original root. And finally it returns a pointer to
 * the root.
 */

Node *
add_product_helper (Node *origin_root, Node *root, char *name, int quantity)
{
  if (root == NULL)
  {
    return new_node (name, quantity);
  }
  if (strcmp (root->product.name, name) == 0)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, PRODUCT_EXISTS);
    return root;
  }
  if (strcmp (root->product.name, name) < 0)
  {
    root->right_child = add_product_helper (origin_root,
                                            root->right_child,
                                            name, quantity);
  }
  if (strcmp (root->product.name, name) > 0)
  {
    root->left_child = add_product_helper (origin_root,
                                           root->left_child,
                                           name, quantity);
  }
  return root;
}

Node *add_product (Node *root, char *name, int quantity)
{
  if (name == NULL)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, INVALID_POINTER);
    return root;
  }
  if (quantity <= 0) // Checking the correctness of the quantity.
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, INVALID_QUANTITY);
    return root;
  }
  Node *temp = add_product_helper (root, root, name, quantity);
  if (root == NULL)
  {
    return temp;
  }
  return root;
}

/**
 * A function that checks whether a string contains only numbers or not.
 * @param c: A string.
 * @return : 0 if True, 1 if false.
 */

int only_digit (const char *c)
{
  for (int i = 0; c[i] != '\0'; ++i)
  {
    if ((i == 0) && (c[0] == '-'))
    {
      continue;
    }
    else if (isdigit(c[i]) == 0)
    {
      return 1;
    }
  }
  return 0;
}

/**
 * A function that checks the correctness of a line.
 * @param line: A line to check.
 * @param pc: Product.
 * @return Updated product, Null if the line is invalid.
 */

Product *parse_product (char *line, Product *pc)
{
  char *quantity_char = NULL;
  char *name = strtok (line, ":");
  if (name == NULL)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, INVALID_LINE);
    return NULL;
  }
  pc->name = name;
  quantity_char = strtok (NULL, ": ""\n");
  if (only_digit (quantity_char))
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, INVALID_LINE);
    return NULL;
  }
  else
  {
    int quantity_int = strtol (quantity_char, NULL, 10);
    pc->quantity = quantity_int;
  }
  return pc;
}

Node *build_bst (const char *filename)
{
  if (!filename)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, FILE_OPEN_FAILED);
    return NULL;
  }
  FILE *in_file = fopen (filename, "r");
  if (in_file == NULL)
  {
    fclose (in_file);
    fprintf (stderr, ERROR);
    fprintf (stderr, FILE_OPEN_FAILED);
    return NULL;
  }
  char line[MAX_LINE_LENGTH] = {0};
  Product pc;
  Node *cur_node, *root;
  int counter = 0;
  while (fgets (line, MAX_LINE_LENGTH, in_file))
  {
    if ((parse_product (line, &pc) != NULL))
    {
      if (!counter)
      {
        cur_node = new_node (pc.name, pc.quantity);
        root = cur_node;
        counter++;
      }
      else
      {
        Node *change = add_product (root, pc.name,
                                    pc.quantity);
        if (!change)
        {
          delete_tree (root);
        }
      }
    }
  }
  fclose (in_file);
  return root;
}

/**
 * A function that finds the successor.
 * @param node: The root or right subtree.
 * @return The successor.
 */

Node *find_min_value (Node *node)
{
  Node *cur_node = node;
  if (cur_node->left_child != NULL)
  {
    while (cur_node && cur_node->left_child->left_child != NULL)
    {
      cur_node = cur_node->left_child;
    }
    Node *ret = cur_node->left_child;
    cur_node->left_child = cur_node->left_child->right_child;
    return ret;
  }
  return cur_node;
}

/**
 * Auxiliary function to the delete product.
 * @param root: Root of the tree.
 * @param name: The name of the product.
 * @return Update tree.
 */

Node *helper_delete_product (Node *root, char *name)
{
  if (root == NULL) // stop conditions
  {
    return root;
  }
  // Finding the product.
  if (strcmp (root->product.name, name) < 0)
  {
    root->right_child = helper_delete_product (root->right_child, name);
  }
  else if (strcmp (root->product.name, name) > 0)
  {
    root->left_child = helper_delete_product (root->left_child, name);
  }
    // If the vertex has no children or one child.
  else
  {
    if (root->left_child == NULL)
    {
      Node *change = root->right_child;
      free_node (root);
      root = NULL;
      return change;
    }
    else if (root->right_child == NULL)
    {
      Node *change = root->left_child;
      free_node (root);
      root = NULL;
      return change;
    }
      // If the vertex has two children.
    else
    {
      Node *change = find_min_value (root->right_child);
      if (root->right_child->left_child != NULL)
      {
        change->right_child = root->right_child;
      }
      change->left_child = root->left_child;
      free_node (root);
      //root = NULL;
      return change;
    }
  }
  return root;
}

Node *delete_product (Node *root, char *name)
{
  if (name == NULL)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, INVALID_POINTER);
    return root;
  }
  if (root == NULL)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, PRODUCT_NOT_FOUND);
    return root;
  }
  Product *check_found = search_product (root, name);
  if (check_found == NULL)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, PRODUCT_NOT_FOUND);
    return root;
  }
  Node *update = helper_delete_product (root, name);
  return update;
}

Product *search_product (Node *root, char *name)
{
  if (name == NULL)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, PRODUCT_NOT_FOUND);
    return NULL;
  }
  while (root != NULL)
  {
    if (strcmp (root->product.name, name) < 0)
    {
      root = root->right_child;
    }
    else if (strcmp (root->product.name, name) > 0)
    {
      root = root->left_child;
    }
    else
    {
      return &root->product;
    }
  }
  return NULL;
}

/**
 * A helper function that deletes name and node.
 * @param node
 */

void free_node (Node *node)
{
  free (node->product.name);
  node->product.name = NULL;
  free (node);
  node = NULL;
}

void delete_tree (Node *root)
{
  if (root == NULL)
  {
    return;
  }
  delete_tree (root->right_child);
  delete_tree (root->left_child);
  free_node (root);
}

Node *update_quantity (Node *root, char *name, int amount_to_update)
{
  if (name == NULL)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, INVALID_POINTER);
    return root;
  }
  if (root == NULL)
  {
    fprintf (stderr, ERROR);
    return root;
  }
  if (!search_product (root, name))
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, PRODUCT_NOT_FOUND);
    return root;
  }
  Product *pc = search_product (root, name);
  int update_val = pc->quantity + amount_to_update;
  if (update_val > 0)
  {
    pc->quantity = update_val;
    return root;
  }
  else if (update_val < 0)
  {
    fprintf (stderr, ERROR);
    fprintf (stderr, INVALID_QUANTITY);
    return root;
  }
  else if (update_val == 0)
  {
    delete_product (root, name);
    return root;
  }
  return root;
}

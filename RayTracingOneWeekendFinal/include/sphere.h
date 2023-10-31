#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"  // Inclui a definição da classe "hittable"
#include "vec3.h"      // Inclui a definição da classe "vec3"

class sphere : public hittable {
public:
    sphere() {}  // Construtor padrão para a classe "sphere"
    sphere(point3 cen, double r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};  // Construtor com parâmetros
    sphere(point3 cen, double r) : center(cen), radius(r) {};  // Construtor com parâmetros

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;  // Função de interseção

public:
    point3 center;  // Centro da esfera
    double radius;  // Raio da esfera
    shared_ptr<material> mat_ptr;  // Ponteiro para o material da esfera
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;  // Vetor do centro da esfera à origem do raio
    auto a = r.direction().length_squared();  // Quadrado do comprimento da direção do raio
    auto half_b = dot(oc, r.direction());  // Metade do produto escalar entre oc e a direção do raio
    auto c = oc.length_squared() - radius * radius;  // Quadrado do comprimento de oc menos o quadrado do raio
    auto discriminant = half_b * half_b - a * c;  // Discriminante da equação quadrática

    if (discriminant < 0)  // Verifica se não há interseção com a esfera
        return false;

    auto sqrtd = sqrt(discriminant);  // Raiz quadrada do discriminante

    // Encontra a raiz mais próxima que está no intervalo aceitável.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;  // Atribui o parâmetro t do ponto de interseção
    rec.p = r.at(rec.t);  // Calcula o ponto de interseção no raio
    vec3 outward_normal = (rec.p - center) / radius;  // Normalizada para fora da esfera
    rec.set_face_normal(r, outward_normal);  // Define a normal e a orientação da face
    rec.mat_ptr = mat_ptr;  // Atribui o material à estrutura de registro de interseção
    return true;  // Indica que houve interseção
}

#endif
